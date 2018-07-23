#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ws2812b.h"
#include "disp.h"
#include "systick.h"


void Fire2012(void);


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();
    WS2812B_init();

    
    while (true){
        Fire2012();

        WS2812B_update();
        nrf_delay_ms(16);
    }
}


uint16_t rand16seed = 1337;

uint8_t random8(void)
{
    rand16seed = (rand16seed * 2053) + 13849;
    // return the sum of the high and low bytes, for better
    //  mixing and non-sequential correlation
    return (uint8_t)(((uint8_t)(rand16seed & 0xFF)) +
                     ((uint8_t)(rand16seed >> 8)));
}

/// Generate an 8-bit random number between 0 and lim
/// @param lim the upper bound for the result
uint8_t random8_1(uint8_t lim)
{
    uint8_t r = random8();
    r = (r*lim) >> 8;
    return r;
}

/// Generate an 8-bit random number in the given range
/// @param min the lower bound for the random number
/// @param lim the upper bound for the random number
uint8_t random8_2(uint8_t min, uint8_t lim)
{
    uint8_t delta = lim - min;
    uint8_t r = random8_1(delta) + min;
    return r;
}


bool gReverseDirection = false;


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  55

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120


/// subtract one byte from another, saturating at 0x00
/// @returns i - j with a floor of 0
uint8_t qsub8( uint8_t i, uint8_t j)
{
    int t = i - j;
    if( t < 0)
        t = 0;
    return t;
}

uint8_t qadd8( uint8_t i, uint8_t j)
{
    unsigned int t = i + j;
    if( t > 255) t = 255;
    return t;
}

tRGB HeatColor( uint8_t temperature)
{
    tRGB heatcolor;

    // Scale 'heat' down from 0-255 to 0-191,
    // which can then be easily divided into three
    // equal 'thirds' of 64 units each.
    uint8_t t192 = scale8_video(temperature, 191);

    // calculate a value that ramps up from
    // zero to 255 in each 'third' of the scale.
    uint8_t heatramp = t192 & 0x3F; // 0..63
    heatramp <<= 2; // scale up to 0..252

    // now figure out which third of the spectrum we're in:
    if( t192 & 0x80) {
        // we're in the hottest third
        heatcolor.R = 255; // full red
        heatcolor.G = 255; // full green
        heatcolor.B = heatramp; // ramp up blue

    } else if( t192 & 0x40 ) {
        // we're in the middle third
        heatcolor.R = 255; // full red
        heatcolor.G = heatramp; // ramp up green
        heatcolor.B = 0; // no blue
    } else {
        // we're in the coolest third
        heatcolor.R = heatramp; // ramp up red
        heatcolor.G = 0; // no green
        heatcolor.B = 0; // no blue
    }

    return heatcolor;
}


void Fire2012(void)
{
    // Array of temperature readings at each simulation cell
    static uint8_t heat[WS2812B_NUMLEDS];

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        heat[i] = qsub8( heat[i],  random8_2(0, ((COOLING * 10) / WS2812B_NUMLEDS) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= WS2812B_NUMLEDS - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
        int y = random8_1(7);
        heat[y] = qadd8( heat[y], random8_2(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < WS2812B_NUMLEDS; j++) {
        tRGB color = HeatColor(heat[j]);

        int pixelnumber;

        if( gReverseDirection ) {
            pixelnumber = (WS2812B_NUMLEDS-1) - j;
        } else {
            pixelnumber = j;
        }

        WS2812B_setColorRGB(&color, pixelnumber, false);
    }
}
