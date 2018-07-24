#include "power.h"
#include "nrfx_saadc.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"


#define ADC_2_MILIVOLT(ADC)   (ADC*1000/4551)


//----------------------------------------------------------------------------------
// Varable Definitions
volatile uint16_t gBatteryMilivolt;
static const nrfx_timer_t    m_timer = NRFX_TIMER_INSTANCE(0);
static nrf_saadc_value_t     saadc_value;
static nrf_ppi_channel_t     m_ppi_channel;


//----------------------------------------------------------------------------------
// Function Declarations
static void timer_handler(nrf_timer_event_t event_type, void * p_context);
static void saadc_callback(nrfx_saadc_evt_t const * p_event);
static void saadc_init(void);
static void saadc_sampling_event_init(void);
static void saadc_sampling_event_enable(void);
static uint8_t millivolt_2_percent(const uint16_t mvolts);


void POWER_init(void)
{
	// Config Power lock pin
    nrf_gpio_cfg_output(POWER_LOCK_PIN);
    nrf_gpio_pin_clear(POWER_LOCK_PIN);

    // Config charging state pin
    nrf_gpio_cfg_input(CHRG_STATE_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CHRG_STDBY_PIN, NRF_GPIO_PIN_PULLUP);

    // Config Battery ADC
    saadc_init();
    saadc_sampling_event_init();
    saadc_sampling_event_enable();
}

uint8_t POWER_battery_level(void)
{
    return millivolt_2_percent(gBatteryMilivolt);
}

static void timer_handler(nrf_timer_event_t event_type, void * p_context)
{

}

static void saadc_callback(nrfx_saadc_evt_t const * p_event)
{
    ret_code_t err_code;
    static uint8_t calib_cnt = 0;

    if(p_event->type == NRFX_SAADC_EVT_DONE){
        if(calib_cnt ++ > 100){
            calib_cnt = 0;
            nrfx_saadc_calibrate_offset();
        }else{
            err_code = nrfx_saadc_buffer_convert(&saadc_value, 1);
            APP_ERROR_CHECK(err_code);

            uint16_t milivolt = 2 * ADC_2_MILIVOLT(p_event->data.done.p_buffer[0]);

            gBatteryMilivolt = (gBatteryMilivolt * 9 + milivolt) / 10;
        }
    }
}

static void saadc_init(void)
{
    ret_code_t err_code;
    nrfx_saadc_config_t saadc_config = {
        .resolution         = NRF_SAADC_RESOLUTION_14BIT,
        .oversample         = NRF_SAADC_OVERSAMPLE_DISABLED,
        .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
        .low_power_mode     = false,  
    };

    nrf_saadc_channel_config_t channel_config = {
        .resistor_p = NRF_SAADC_RESISTOR_DISABLED,
        .resistor_n = NRF_SAADC_RESISTOR_DISABLED,
        .gain       = NRF_SAADC_GAIN1_6,
        .reference  = NRF_SAADC_REFERENCE_INTERNAL,
        .acq_time   = NRF_SAADC_ACQTIME_40US,
        .mode       = NRF_SAADC_MODE_SINGLE_ENDED,
        .burst      = NRF_SAADC_BURST_DISABLED,
        .pin_p      = (nrf_saadc_input_t)(NRF_SAADC_INPUT_AIN3),
        .pin_n      = NRF_SAADC_INPUT_DISABLED,
    };

    err_code = nrfx_saadc_init(&saadc_config, saadc_callback);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_channel_init(3, &channel_config);
    APP_ERROR_CHECK(err_code);

    // Block Read
    nrfx_saadc_sample_convert(3, &saadc_value);
    gBatteryMilivolt = ADC_2_MILIVOLT(saadc_value);
}

static void saadc_sampling_event_init(void)
{
    ret_code_t err_code;

    nrfx_timer_config_t timer_cfg = {
        .frequency          = NRF_TIMER_FREQ_500kHz,
        .mode               = NRF_TIMER_MODE_TIMER,
        .bit_width          = NRF_TIMER_BIT_WIDTH_32,
        .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
        .p_context          = NULL,
    };

    err_code = nrfx_timer_init(&m_timer, &timer_cfg, timer_handler);
    APP_ERROR_CHECK(err_code);

    /* setup m_timer for compare event every 1000ms */
    uint32_t ticks = nrfx_timer_ms_to_ticks(&m_timer, 1000);
    nrfx_timer_extended_compare(&m_timer,
                                   NRF_TIMER_CC_CHANNEL0,
                                   ticks,
                                   NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK,
                                   false);
    nrfx_timer_enable(&m_timer);

    uint32_t timer_compare_event_addr = nrfx_timer_compare_event_address_get(&m_timer, NRF_TIMER_CC_CHANNEL0);
    uint32_t saadc_sample_task_addr   = nrfx_saadc_sample_task_get();

    /* setup ppi channel so that timer compare event is triggering sample task in SAADC */
    err_code = nrfx_ppi_channel_alloc(&m_ppi_channel);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_ppi_channel_assign(m_ppi_channel,
                                          timer_compare_event_addr,
                                          saadc_sample_task_addr);
    APP_ERROR_CHECK(err_code);
}

static void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrfx_saadc_buffer_convert(&saadc_value, 1);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
}

/****************************************************************************************
 * @brief Function for converting the input voltage (in milli volts) into percentage of 3.0 Volts.
 *
 * @details The calculation is based on a linearized version of the battery's discharge
 *           curve. 3.0V returns 100% battery level. The limit for power failure is 2.1V and
 *           is considered to be the lower boundary.
 *
 *           The discharge curve for CR2032 is non-linear. In this model it is split into
 *           4 linear sections:
 *           - Section 1: 4.2V - 3.7V = 100% - 50% (50% drop on 500 mV)
 *           - Section 2: 3.7V - 3.5V = 50% - 15% (35% drop on 200 mV)
 *           - Section 3: 3.5V - 3.3V = 15% - 5% (10% drop on 200 mV)
 *           - Section 4: 3.3V - 2.8V = 5% - 0% (5% drop on 500 mV)
 *
 *           These numbers are by no means accurate. Temperature and
 *           load in the actual application is not accounted for!
 *
 * @param[in] mvolts The voltage in mV
 *
 * @return    Battery level in percent.
***************************************************************************************/
static uint8_t millivolt_2_percent(const uint16_t mvolts)
{
    uint8_t battery_level;

    if (mvolts >= 4200){
        battery_level = 100;
    }else if (mvolts > 3700){
        battery_level = 100 - ((4200 - mvolts) * 50) / 500;
    }else if (mvolts > 3500){
        battery_level = 50 - ((3700 - mvolts) * 35) / 200;
    }else if (mvolts > 3300){
        battery_level = 15 - ((3500 - mvolts) * 10) / 200;
    }else if (mvolts > 2800){
        battery_level = 5 - ((3300 - mvolts) * 5) / 500;
    }else{
        battery_level = 0;
    }

    return battery_level;
}
