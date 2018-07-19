
// Cylon

void scale_rgb(tRGB * rgb, uint8_t scale)
{
    uint16_t scale_fixed = scale + 1;
    rgb->R = (((uint16_t)rgb->R) * scale_fixed) >> 8;
    rgb->G = (((uint16_t)rgb->G) * scale_fixed) >> 8;
    rgb->B = (((uint16_t)rgb->B) * scale_fixed) >> 8;
}

void fadeall() 
{ 
    tRGB rgb;

    for(int i = 0; i < WS2812B_NUMLEDS; i++) { 
        rgb = WS2812B_getColorRGB(i);
        scale_rgb(&rgb, 250);
        WS2812B_setColorRGB(&rgb, i, false);
    } 
}

int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    tRGB rgb;
    tHSV hsv = {0, 255, 255};

    while (true){
        static uint8_t hue = 0;

        // First slide the led in one direction
        for(int i = 0; i < WS2812B_NUMLEDS; i++) {
            hsv.H = hue++;
            hsv2rgb(&hsv, &rgb);
            WS2812B_setColorRGB(&rgb, i, false);
            WS2812B_update();

            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fadeall();

            // Wait a little bit before we loop around and do it again
            nrf_delay_ms(10);
        }

        // Now go in the other direction.  
        for(int i = (WS2812B_NUMLEDS)-1; i >= 0; i--) {
            hsv.H = hue++;
            hsv2rgb(&hsv, &rgb);
            WS2812B_setColorRGB(&rgb, i, false);
            WS2812B_update();

            // now that we've shown the leds, reset the i'th led to black
            // leds[i] = CRGB::Black;
            fadeall();

            // Wait a little bit before we loop around and do it again
            nrf_delay_ms(10);
        }
    }
}