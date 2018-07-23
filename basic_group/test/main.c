#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "systick.h"
#include "led.h"
#include "fastled.h"


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();
    LED_init();

    tHSV hsv = {0, 255, 255};

    while (true){
        static uint8_t hue = 0;

        // First slide the led in one direction
        for(int i = 0; i < LED_NUM; i++) {
            hsv.H = hue++;
            hsv2rgb(&hsv, &gLED[i]);
            LED_show();

            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fadeToBlackBy(gLED, LED_NUM, 10);

            // Wait a little bit before we loop around and do it again
            nrf_delay_ms(10);
        }

        // Now go in the other direction.  
        for(int i = (LED_NUM)-1; i >= 0; i--) {
            hsv.H = hue++;
            hsv2rgb(&hsv, &gLED[i]);
            LED_show();

            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fadeToBlackBy(gLED, LED_NUM, 10);

            // Wait a little bit before we loop around and do it again
            nrf_delay_ms(10);
        }
    }
}
