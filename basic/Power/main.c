#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrfx_saadc.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"
#include "nrf_gpio.h"


#define POWER_LOCK_PIN  6
#define CHRG_STATE_PIN  3
#define CHRG_STDBY_PIN  4


#define ADC_2_MILIVOLT(ADC)   (ADC*1000/4551)

static const nrfx_timer_t    m_timer = NRFX_TIMER_INSTANCE(0);
static nrf_saadc_value_t     saadc_value;
static nrf_ppi_channel_t     m_ppi_channel;


void timer_handler(nrf_timer_event_t event_type, void * p_context)
{

}

void saadc_callback(nrfx_saadc_evt_t const * p_event)
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

            int16_t adc_val = ADC_2_MILIVOLT(p_event->data.done.p_buffer[0]);
            int16_t bat_val = adc_val * 2;
            NRF_LOG_INFO("ADC milivolt: %d mV, Battery: %d mV", adc_val, bat_val);
        }
    }
}

void saadc_init(void)
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
    NRF_LOG_INFO("Start up Block Read milivolt: %d mV", ADC_2_MILIVOLT(saadc_value));
}

void saadc_sampling_event_init(void)
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

void saadc_sampling_event_enable(void)
{
    ret_code_t err_code = nrfx_saadc_buffer_convert(&saadc_value, 1);
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_ppi_channel_enable(m_ppi_channel);
    APP_ERROR_CHECK(err_code);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // Config Power lock pin
    nrf_gpio_cfg_output(POWER_LOCK_PIN);

    // Config charging state pin
    nrf_gpio_cfg_input(CHRG_STATE_PIN, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(CHRG_STDBY_PIN, NRF_GPIO_PIN_PULLUP);

    // Config Battery ADC
    saadc_init();
    saadc_sampling_event_init();
    saadc_sampling_event_enable();

    int cnt = 0;

    // Lock Power supply
    nrf_gpio_pin_set(POWER_LOCK_PIN);

    while (true){
        nrf_delay_ms(1000);

        if(nrf_gpio_pin_read(CHRG_STATE_PIN) && nrf_gpio_pin_read(CHRG_STDBY_PIN)){
            NRF_LOG_DEBUG("No Charger")
        }else if(!nrf_gpio_pin_read(CHRG_STATE_PIN) && nrf_gpio_pin_read(CHRG_STDBY_PIN)){
            NRF_LOG_DEBUG("Charging")
        }else if(nrf_gpio_pin_read(CHRG_STATE_PIN) && !nrf_gpio_pin_read(CHRG_STDBY_PIN)){
            NRF_LOG_DEBUG("Charging Complete")
        }

        if(cnt < 20){
            cnt ++;
        }else if(cnt == 20){
            cnt ++;
            // Unlock Power supply
            nrf_gpio_pin_clear(POWER_LOCK_PIN);
        }
    }
}
