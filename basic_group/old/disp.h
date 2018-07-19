#ifndef _DISP_H_
#define _DISP_H_


#include "nrf.h"
#include "ws2812b.h"


typedef struct sHSV{
	int16_t H;		// Hue
	uint8_t S;		// Saturation
	uint8_t V;		// Value
}tHSV;


void DISP_add_back(tHSV hsv);

void DISP_add_point(tHSV hsv, int pointSize, int pointCG, int pointPosition);

static inline void DISP_update(void){WS2812B_update();}

void DISP_rgb2hsv(tRGB *pRGB, tHSV *pHSV);

void DISP_hsv2rgb(tHSV *pHSV, tRGB *pRGB);


#endif
