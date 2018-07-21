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
        hsv2rgb_rainbow(&hsv, &rgb);
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

void confetti(void)
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

void nscale8( tRGB * rgb, uint8_t num_leds, uint8_t scale)
{
    for( uint8_t i = 0; i < num_leds; i++) {
        rgb->R = scale8(rgb->R, scale);
        rgb->G = scale8(rgb->G, scale);
        rgb->B = scale8(rgb->B, scale);
    }
}

void fadeToBlackBy( tRGB * rgb, uint16_t num_leds, uint8_t fadeBy)
{
    nscale8( rgb, num_leds, 255 - fadeBy);
}


void rainbow();
void rainbowWithGlitter(void);


uint8_t Hue = 0;

int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    

    while (true){
        //rainbow();
        //rainbowWithGlitter();
        confetti();

        WS2812B_update();
        nrf_delay_ms(10);
        Hue ++;
    }
}

void rainbow(void) 
{
    fill_rainbow(Hue, 7);
}

void rainbowWithGlitter(void) 
{
    // built-in FastLED rainbow, plus some random sparkly glitter
    rainbow();
    addGlitter(80);
}

void sinelon()
{
    // a colored dot sweeping back and forth, with fading trails
    tRGB rgb;

    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        rgb = WS2812B_getColorRGB(i);
        fadeToBlackBy( &rgb, 1, 20);
        WS2812B_setColorRGB(&rgb, i, false);
    }

    int pos = beatsin16( 13, 0, NUM_LEDS-1 );
    leds[pos] += CHSV( gHue, 255, 192);
}
