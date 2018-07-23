#include "video.h"


//----------------------------------------------------------------------------------
// Varable Definitions


//----------------------------------------------------------------------------------
// Function Declarations


void VIDEO_cylon(void)
{
	static uint32_t tick = 0;
	static int8_t cnt = 0;
	static int8_t direc = 1;
    static tHSV hsv = {0, 255, 255};

    if(SYSTICK_get_ms_since(tick) < 10){
    	return;
    }
    tick = SYSTICK_get_tick();

	cnt += direc;
    hsv.H ++;
    hsv2rgb(&hsv, &gLED[cnt]);
    LED_show();
    fadeToBlackBy(gLED, LED_NUM, 10);

    if(cnt >= LED_NUM){
    	direc = -1;
    }else if(cnt < 0){
    	direc = +1;
    }
}

void VIDEO_fill_from_palette(const tRGB * pal)
{
    static uint32_t tick = 0;
    static uint8_t colorIndex = 0;

    if(SYSTICK_get_ms_since(tick) < 10){
        return;
    }
    tick = SYSTICK_get_tick();

    colorIndex ++;

    uint8_t hue = colorIndex;
    for( int i = 0; i < LED_NUM; i++) {
        gLED[i] = ColorFromPalette(pal, hue, 255, LINEARBLEND);
        hue += 3;
    }
    LED_show();
}
