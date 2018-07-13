#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "ws2812b.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    tColor color = {.R = 0, .G = 0, .B = 0,};

    color.R = 100;
    color.G = 0;
    color.B = 0;
    for(int i=0; i<16; i++){
    	WS2812B_setColorRGB(&color, i, true);
    }

    nrf_delay_ms(3000);

    color.R = 0;
    color.G = 100;
    color.B = 0;
    for(int i=0; i<16; i++){
    	WS2812B_setColorRGB(&color, i, true);
    }

    nrf_delay_ms(3000);

    color.R = 0;
    color.G = 0;
    color.B = 100;
    for(int i=0; i<16; i++){
    	WS2812B_setColorRGB(&color, i, true);
    }

    nrf_delay_ms(3000);
    
    color.R = 0;
    color.G = 0;
    color.B = 0;
    for(int i=0; i<16; i++){
        WS2812B_setColorRGB(&color, i, true);
    }

    while (true){
        nrf_delay_ms(1000);

        NRF_LOG_DEBUG("Hello World");
    }
}
