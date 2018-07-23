#ifndef _WS2812B_H_
#define _WS2812B_H_


#include "nrfx_pwm.h"


#define WS2812B_PIN	    	   11
#define WS2812B_NUMLEDS        16
#define WS2812B_BITS_PER_LED   24


void WS2812B_init(void);
void WS2812B_setColorRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t index, bool isShow);
void WS2812B_update(uint8_t * pbuf);
void WS2812B_show(void);


#endif
