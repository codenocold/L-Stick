#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "ws2812b.h"
#include "disp.h"
#include "systick.h"


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
void sinelon(void);
void bpm(void);

uint8_t Hue = 0;

int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();
    WS2812B_init();

    

    while (true){
        //rainbow();
        //rainbowWithGlitter();
        //confetti();
        //sinelon();
        //bpm();
        juggle();

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

// beat16 generates a 16-bit 'sawtooth' wave at a given BPM,
///        with BPM specified in Q8.8 fixed-point format; e.g.
///        for this function, 120 BPM MUST BE specified as
///        120*256 = 30720.
///        If you just want to specify "120", use beat16 or beat8.
uint16_t beat88( uint16_t beats_per_minute_88, uint32_t timebase)
{
    // BPM is 'beats per minute', or 'beats per 60000ms'.
    // To avoid using the (slower) division operator, we
    // want to convert 'beats per 60000ms' to 'beats per 65536ms',
    // and then use a simple, fast bit-shift to divide by 65536.
    //
    // The ratio 65536:60000 is 279.620266667:256; we'll call it 280:256.
    // The conversion is accurate to about 0.05%, more or less,
    // e.g. if you ask for "120 BPM", you'll get about "119.93".
    return ((SYSTICK_get_tick() - timebase) * beats_per_minute_88 * 280) >> 16;
}

/// beat16 generates a 16-bit 'sawtooth' wave at a given BPM
uint16_t beat16( uint16_t beats_per_minute, uint32_t timebase)
{
    // Convert simple 8-bit BPM's to full Q8.8 accum88's if needed
    if( beats_per_minute < 256) beats_per_minute <<= 8;
    return beat88(beats_per_minute, timebase);
}

/// Fast 16-bit approximation of sin(x). This approximation never varies more than
/// 0.69% from the floating point value you'd get by doing
///
///     float s = sin(x) * 32767.0;
///
/// @param theta input angle from 0-65535
/// @returns sin of theta, value between -32767 to 32767.
int16_t sin16( uint16_t theta )
{
    static const uint16_t base[] =
    { 0, 6393, 12539, 18204, 23170, 27245, 30273, 32137 };
    static const uint8_t slope[] =
    { 49, 48, 44, 38, 31, 23, 14, 4 };

    uint16_t offset = (theta & 0x3FFF) >> 3; // 0..2047
    if( theta & 0x4000 ) offset = 2047 - offset;

    uint8_t section = offset / 256; // 0..7
    uint16_t b   = base[section];
    uint8_t  m   = slope[section];

    uint8_t secoffset8 = (uint8_t)(offset) / 2;

    uint16_t mx = m * secoffset8;
    int16_t  y  = mx + b;

    if( theta & 0x8000 ) y = -y;

    return y;
}

/// scale a 16-bit unsigned value by a 16-bit value,
///         considered as numerator of a fraction whose denominator
///         is 65536. In other words, it computes i * (scale / 65536)
uint16_t scale16( uint16_t i, uint16_t scale )
{
    uint16_t result;

    result = ((uint32_t)(i) * (1+(uint32_t)(scale))) / 65536;

    return result;
}

/// beatsin16 generates a 16-bit sine wave at a given BPM,
///           that oscillates within a given range.
uint16_t beatsin16( uint16_t beats_per_minute, uint16_t lowest, uint16_t highest,
                               uint32_t timebase, uint16_t phase_offset)
{
    uint16_t beat = beat16( beats_per_minute, timebase);
    uint16_t beatsin = (sin16( beat + phase_offset) + 32768);
    uint16_t rangewidth = highest - lowest;
    uint16_t scaledbeat = scale16( beatsin, rangewidth);
    uint16_t result = lowest + scaledbeat;
    return result;
}

void sinelon(void)
{
    // a colored dot sweeping back and forth, with fading trails
    tRGB rgb;
    tHSV hsv;

    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        rgb = WS2812B_getColorRGB(i);
        fadeToBlackBy( &rgb, 1, 20);
        WS2812B_setColorRGB(&rgb, i, false);
    }

    int pos = beatsin16(13, 0, 16-1, 0, 0);

    hsv.H = Hue;
    hsv.S = 255;
    hsv.V = 192;
    hsv2rgb_rainbow(&hsv, &rgb);

    tRGB trgb;
    trgb = WS2812B_getColorRGB(pos);

    unsigned int t;

    t = trgb.R + rgb.R;
    if( t > 255){
        t = 255;
    }
    trgb.R = t;

    t = trgb.G + rgb.G;
    if( t > 255){
        t = 255;
    }
    trgb.G = t;

    t = trgb.B + rgb.B;
    if( t > 255){
        t = 255;
    }
    trgb.B = t;

    WS2812B_setColorRGB(&trgb, pos, false);
}

const uint8_t b_m16_interleave[] = { 0, 49, 49, 41, 90, 27, 117, 10 };

/// Fast 8-bit approximation of sin(x). This approximation never varies more than
/// 2% from the floating point value you'd get by doing
///
///     float s = (sin(x) * 128.0) + 128;
///
/// @param theta input angle from 0-255
/// @returns sin of theta, value between 0 and 255
uint8_t sin8( uint8_t theta)
{
    uint8_t offset = theta;
    if( theta & 0x40 ) {
        offset = (uint8_t)255 - offset;
    }
    offset &= 0x3F; // 0..63

    uint8_t secoffset  = offset & 0x0F; // 0..15
    if( theta & 0x40) secoffset++;

    uint8_t section = offset >> 4; // 0..3
    uint8_t s2 = section * 2;
    const uint8_t* p = b_m16_interleave;
    p += s2;
    uint8_t b   =  *p;
    p++;
    uint8_t m16 =  *p;

    uint8_t mx = (m16 * secoffset) >> 4;

    int8_t y = mx + b;
    if( theta & 0x80 ) y = -y;

    y += 128;

    return y;
}

/// beatsin8 generates an 8-bit sine wave at a given BPM,
///           that oscillates within a given range.
uint8_t beatsin8( uint16_t beats_per_minute, uint8_t lowest, uint8_t highest,
                            uint32_t timebase, uint8_t phase_offset)
{
    uint8_t beat = beat16( beats_per_minute, timebase) >> 8;
    uint8_t beatsin = sin8( beat + phase_offset);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8( beatsin, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

void bpm(void)
{
    tRGB rgb;

    // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
    uint8_t BeatsPerMinute = 62;
    uint8_t beat = beatsin8( BeatsPerMinute, 64, 255, 0, 0);
    for( int i = 0; i < 16; i++) { //9948
        rgb = ColorFromPalette(CloudColors_p, Hue+(i*2), beat-Hue+(i*10), LINEARBLEND);
        WS2812B_setColorRGB(&rgb, i, false);
    }
}

void juggle() {
    // eight colored dots, weaving in and out of sync with each other
    tRGB rgb;
    tHSV hsv;

    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        rgb = WS2812B_getColorRGB(i);
        fadeToBlackBy( &rgb, 1, 20);
        WS2812B_setColorRGB(&rgb, i, false);
    }

    tRGB trgb;
    uint8_t dothue = 0;
    for( int i = 0; i < 8; i++) {
        hsv.H = dothue;
        hsv.S = 200;
        hsv.V = 255;
        hsv2rgb_rainbow(&hsv, &rgb);

        int pos = beatsin16(i+7, 0, WS2812B_NUMLEDS-1, 0, 0);
        trgb = WS2812B_getColorRGB(pos);

        if(trgb.R < rgb.R){
            trgb.R = rgb.R;
        }
        if(trgb.G < rgb.G){
            trgb.G = rgb.G;
        }
        if(trgb.B < rgb.B){
            trgb.B = rgb.B;
        }

        WS2812B_setColorRGB(&trgb, pos, false);

        dothue += 32;
    }
}