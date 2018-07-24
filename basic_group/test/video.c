#include "video.h"


//----------------------------------------------------------------------------------
// Varable Definitions
static int16_t mCnt = 0;
static int8_t  mInc = 1;
static uint8_t mHue = 0;


//----------------------------------------------------------------------------------
// Function Declarations


#define UPDATES_PER_SECOND(Hz) {    \
    static uint32_t tick = 0;       \
    if(SYSTICK_get_ms_since(tick) < 1000/Hz){   \
        return;                     \
    }                               \
    tick = SYSTICK_get_tick();      \
}


void VIDEO_cylon(void)
{
    UPDATES_PER_SECOND(100);

    mHue ++;

	mCnt += mInc;
    if(mCnt >= LED_NUM){
        mInc = -1;
    }else if(mCnt < 0){
        mInc = +1;
    }

    tHSV hsv = {mHue, 255, 255};
    hsv2rgb(&hsv, &gLED[mCnt]);
    fadeToBlackBy(gLED, LED_NUM, 20);

    LED_show();
}

void VIDEO_fill_from_palette(const tRGB * pal)
{
    UPDATES_PER_SECOND(100);

    uint8_t index = mHue ++;
    for( int i = 0; i < LED_NUM; i++) {
        gLED[i] = ColorFromPalette(pal, index, 255, LINEARBLEND);
        index += 3;
    }

    LED_show();
}

void VIDEO_rainbow(uint8_t deltahue)
{
    UPDATES_PER_SECOND(100);

    tHSV hsv = {mHue ++, 255, 255};
    for(int i = 0; i < LED_NUM; i++) {
        hsv2rgb(&hsv, &gLED[i]);
        hsv.H += deltahue;
    }

    LED_show();
}

void VIDEO_confetti(void)
{
    UPDATES_PER_SECOND(100);

    rgb_nscale(gLED, LED_NUM, 240);

    if(mCnt ++ > 20){
        mCnt = 0;
        int pos = random8() % LED_NUM;
        tHSV hsv = {random8(), 200, 255};
        hsv2rgb(&hsv, &gLED[pos]);
    }

    LED_show();
}

void VIDEO_sinelon(void)
{
    UPDATES_PER_SECOND(100);

    fadeToBlackBy(gLED, LED_NUM, 20);

    int pos = beatsin16(13, 0, LED_NUM-1, 0, 0);

    tRGB rgb;
    tHSV hsv = {mHue ++, 255, 255};
    hsv2rgb(&hsv, &rgb);
    rgb_add(&gLED[pos], rgb);

    LED_show();
}

void VIDEO_bpm(void)
{
    UPDATES_PER_SECOND(100);

    mHue ++;
    uint8_t beat = beatsin8( 62, 64, 255, 0, 0);
    for( int i = 0; i < LED_NUM; i++) {
        tRGB rgb = ColorFromPalette(PaletteCloudColors_p, mHue+(i*2), beat-mHue+(i*10), LINEARBLEND);
        gLED[i] = rgb;
    }

    LED_show();
}

void VIDEO_juggle(void)
{
    UPDATES_PER_SECOND(100);

    fadeToBlackBy(gLED, LED_NUM, 20);

    tRGB rgb;
    tHSV hsv = {.H = 0, .S = 200, .V = 255};
    for( int i = 0; i < 8; i++) {
        hsv2rgb(&hsv, &rgb);
        rgb_or(&gLED[beatsin16(i+7, 0, LED_NUM-1, 0, 0)], rgb);
        hsv.H += 32;
    }

    LED_show();
}


// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  30

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 170

tRGB HeatColor(uint8_t temperature)
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

void VIDEO_fire(void)
{
    static bool gReverseDirection = false;

    UPDATES_PER_SECOND(60);

    // Array of temperature readings at each simulation cell
    static uint8_t heat[LED_NUM];

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < LED_NUM; i++) {
        heat[i] = qsub8( heat[i],  random8_2(0, ((COOLING * 10) / LED_NUM) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k=LED_NUM - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
        int y = random8_1(7);
        heat[y] = qadd8( heat[y], random8_2(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < LED_NUM; j++) {
        tRGB color = HeatColor(heat[j]);

        int pixelnumber;

        if( gReverseDirection ) {
            pixelnumber = (LED_NUM-1) - j;
        } else {
            pixelnumber = j;
        }

        gLED[pixelnumber] = color;
    }

    LED_show();
}

void VIDEO_fire_whith_palette(const tRGB * pal)
{
    static bool gReverseDirection = false;

    UPDATES_PER_SECOND(60);

    // Array of temperature readings at each simulation cell
    static uint8_t heat[LED_NUM];

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < LED_NUM; i++) {
        heat[i] = qsub8( heat[i],  random8_2(0, ((COOLING * 10) / LED_NUM) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k=LED_NUM - 1; k >= 2; k--) {
        heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
        int y = random8_1(7);
        heat[y] = qadd8( heat[y], random8_2(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for(int j = 0; j < LED_NUM; j++) {
        uint8_t colorindex = scale8( heat[j], 240);
        tRGB color = ColorFromPalette(pal, colorindex, 255, LINEARBLEND);

        int pixelnumber;

        if( gReverseDirection ) {
            pixelnumber = (LED_NUM-1) - j;
        } else {
            pixelnumber = j;
        }

        gLED[pixelnumber] = color;
    }

    LED_show();
}
