#include "ws2812b.h"


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

static nrf_pwm_values_common_t seq1_values[WS2812B_NUMLEDS * WS2812B_BITS_PER_LED];
nrf_pwm_sequence_t const seq1 = 
{
    .values.p_common = seq1_values,
    .length          = NRF_PWM_VALUES_LENGTH(seq1_values),
    .repeats         = 0,
    .end_delay       = 0
};


/*****************************************************************************
 * @bref        Init ws2812b driver
 * @param[in]   none
 * @retval      none
 *****************************************************************************/
void WS2812B_init(void)
{
	int i;

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

    // Reset Init state
	for(i=0; i< WS2812B_NUMLEDS * WS2812B_BITS_PER_LED; i++){
		seq1_values[i] = LOGIC_ZERO;
	}
}

/*****************************************************************************
 * @bref        Set a led color
 * @param[in]   pColor led color ptr
 * @param[in]   led led number
 * @param[in]   update Perform an update after changing led color
 * @retval      none
 *****************************************************************************/
void WS2812B_setColorRGB(tColor* pColor, uint8_t led, bool update)
{
    if((led >= WS2812B_NUMLEDS)){
        return;
    }

    int offset   = led * WS2812B_BITS_PER_LED;

    uint32_t grb = (pColor->G << 16) | (pColor->R << 8) | (pColor->B);

    for (int bit = (WS2812B_BITS_PER_LED - 1); bit >= 0; --bit) {
        seq1_values[offset++] = (grb & (1 << bit)) ? LOGIC_ONE : LOGIC_ZERO;
    }

    if (update){
        WS2812B_update();
    }
}

/*****************************************************************************
 * @bref        Get a led color
 * @param[in]   led led number
 * @retval      Color
 *****************************************************************************/
tColor WS2812B_getColorRGB(uint8_t led)
{
    tColor color;
    uint32_t grb = 0;
    
    if((led >= WS2812B_NUMLEDS)){
        led = 0;
    }

    int offset = led * WS2812B_BITS_PER_LED;

    for (int bit = (WS2812B_BITS_PER_LED - 1); bit >= 0; --bit) {
        if(seq1_values[offset++] == LOGIC_ONE){
            grb |= (1 << bit);
        }
    }

    color.R = (grb>>8) & 0xFF;
    color.G = (grb>>16) & 0xFF;
    color.B = grb & 0xFF;

    return color;
}

/*****************************************************************************
 * @bref    Update led color
 * @param   none
 * @retval  none
 *****************************************************************************/
void WS2812B_update(void)
{
    // Wait for last operation
    while(!nrfx_pwm_is_stopped(&m_pwm0)){
    }
    
    nrfx_pwm_complex_playback(&m_pwm0, &seq0, &seq1, 1, NRFX_PWM_FLAG_STOP);
}
