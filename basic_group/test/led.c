#include "led.h"


#define WS2812B_PIN	    	   	11
#define WS2812B_BITS_PER_LED   	24  

// Timing Definitions
#define LOGIC_ONE       (14 + 0x8000)
#define LOGIC_ZERO      (6  + 0x8000)
#define LOGIC_RESET     (0  + 0x8000)


static nrfx_pwm_t m_pwm0 = NRFX_PWM_INSTANCE(0);

static nrf_pwm_values_common_t seq0_values[] = { LOGIC_RESET };
nrf_pwm_sequence_t const seq0 = 
{
    .values.p_common = seq0_values,
    .length          = NRF_PWM_VALUES_LENGTH(seq0_values),
    .repeats         = 45,
    .end_delay       = 0
};

static nrf_pwm_values_common_t seq1_values[LED_NUM * WS2812B_BITS_PER_LED];
nrf_pwm_sequence_t const seq1 = 
{
    .values.p_common = seq1_values,
    .length          = NRF_PWM_VALUES_LENGTH(seq1_values),
    .repeats         = 0,
    .end_delay       = 0
};


//----------------------------------------------------------------------------------
// Varable Definitions
tRGB gLED[LED_NUM];


//----------------------------------------------------------------------------------
// Function Declarations


void LED_init(void)
{
	nrfx_pwm_config_t const config =
    {
        .output_pins =
        {
            WS2812B_PIN,                                         // channel 0
            NRFX_PWM_PIN_NOT_USED,                               // channel 1
            NRFX_PWM_PIN_NOT_USED,                               // channel 2
            NRFX_PWM_PIN_NOT_USED,                               // channel 3
        },
        .irq_priority = APP_IRQ_PRIORITY_LOWEST,
        .base_clock   = NRF_PWM_CLK_16MHz,
        .count_mode   = NRF_PWM_MODE_UP,
        .top_value    = 20,
        .load_mode    = NRF_PWM_LOAD_COMMON,
        .step_mode    = NRF_PWM_STEP_AUTO
    };
    APP_ERROR_CHECK(nrfx_pwm_init(&m_pwm0, &config, NULL));

    // Reset data buff
	for(int i=0; i< LED_NUM * WS2812B_BITS_PER_LED; i++){
		seq1_values[i] = LOGIC_ZERO;
	}

	for(int i=0; i<LED_NUM; i++){
		gLED[i].R = 0;
		gLED[i].G = 0;
		gLED[i].B = 0;
	}
}

void LED_setColorRGB(tRGB rgb, uint8_t index, bool isShow)
{
    if((index >= LED_NUM)){
        return;
    }

    int offset   = index * WS2812B_BITS_PER_LED;

    uint32_t grb = (rgb.G << 16) | (rgb.R << 8) | (rgb.B);

    for (int bit = (WS2812B_BITS_PER_LED - 1); bit >= 0; --bit) {
        seq1_values[offset++] = (grb & (1 << bit)) ? LOGIC_ONE : LOGIC_ZERO;
    }

    if (isShow){
        LED_show();
    }
}

void LED_show(void)
{
    uint32_t grb;

    for(int offset=0; offset < (LED_NUM * WS2812B_BITS_PER_LED); ){
        grb = *((uint32_t*)(gLED + offset / WS2812B_BITS_PER_LED));
        for (int bit = (WS2812B_BITS_PER_LED - 1); bit >= 0; --bit){
            seq1_values[offset++] = (grb & (1 << bit)) ? LOGIC_ONE : LOGIC_ZERO;
        }
    }

    // Wait for last operation
    while(!nrfx_pwm_is_stopped(&m_pwm0)){
    }
    
    nrfx_pwm_complex_playback(&m_pwm0, &seq0, &seq1, 1, NRFX_PWM_FLAG_STOP);
}
