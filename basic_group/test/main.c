#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ws2812b.h"
#include "disp.h"


uint16_t rand16seed = 1337;

uint8_t random8(void)
{
    rand16seed = (rand16seed * 2053) + 13849;
    // return the sum of the high and low bytes, for better
    //  mixing and non-sequential correlation
    return (uint8_t)(((uint8_t)(rand16seed & 0xFF)) +
                     ((uint8_t)(rand16seed >> 8)));
}

void fill_rainbow(uint8_t initialhue, uint8_t deltahue)
{
    tHSV hsv;
    tRGB rgb;
    hsv.H = initialhue;
    hsv.V = 255;
    hsv.S = 240;
    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        hsv2rgb(&hsv, &rgb);
        WS2812B_setColorRGB(&rgb, i, false);
        hsv.H += deltahue;
    }
}

void addGlitter(uint8_t chanceOfGlitter) 
{
    uint8_t index;
    tRGB rgb;

    if( random8() < chanceOfGlitter) {
        index = random8() % WS2812B_NUMLEDS;
        rgb = WS2812B_getColorRGB(index);
        rgb.R = 255;
        rgb.G = 255;
        rgb.B = 255;
        WS2812B_setColorRGB(&rgb, index, false);
    }
}

void confetti() 
{
    static int cnt = 0;
    tRGB rgb;

    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        rgb = WS2812B_getColorRGB(i);
        rgb.R = scale8(rgb.R, 255-10);
        rgb.G = scale8(rgb.G, 255-10);
        rgb.B = scale8(rgb.B, 255-10);
        WS2812B_setColorRGB(&rgb, i, false);
    }

    if(cnt ++ > 20){
        cnt = 0;
        int pos = random8() % WS2812B_NUMLEDS;
        tHSV hsv = {0, 200, 255};
        hsv.H = random8();
        hsv2rgb(&hsv, &rgb);
        WS2812B_setColorRGB(&rgb, pos, false);
    }
}


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    uint8_t Hue = 0;

    while (true){
        confetti();

        WS2812B_update();

        nrf_delay_ms(10);

        Hue ++;
    }
}
