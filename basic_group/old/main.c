#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ws2812b.h"
#include "disp.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    tHSV hsv_back = {.H = 100, .S = 255, .V = 0};
    DISP_add_back(hsv_back);

    tHSV hsv_point = {.H = 0, .S = 255, .V = 255};
    DISP_add_point(hsv_point, 10, 500, 7500);
    DISP_update();

    while (true){
        // nrf_delay_ms(1000);
        // color.R = 100;
        // color.G = 0;
        // color.B = 0;
        // for(int i=0; i<WS2812B_NUMLEDS; i++){
        //     WS2812B_setColorRGB(&color, i, false);
        // }
        // WS2812B_update();
        
        // nrf_delay_ms(1000);
        // color.R = 0;
        // color.G = 100;
        // color.B = 0;
        // for(int i=0; i<WS2812B_NUMLEDS; i++){
        //     WS2812B_setColorRGB(&color, i, false);
        // }
        // WS2812B_update();

        // nrf_delay_ms(1000);
        // color.R = 0;
        // color.G = 0;
        // color.B = 100;
        // for(int i=0; i<WS2812B_NUMLEDS; i++){
        //     WS2812B_setColorRGB(&color, i, false);
        // }
        // WS2812B_update();
    }
}
