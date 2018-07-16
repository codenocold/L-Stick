#ifndef _DISP_H_
#define _DISP_H_


#include "nrf.h"
#include "ws2812b.h"


typedef struct sHSV{
	int16_t H;		// Hue
	uint8_t S;		// Saturation
	uint8_t V;		// Value
}tHSV;


/*****************************************************************************
 * @bref        Function for add back to the buf.
 * @param[in]   r
 * @param[in]   g
 * @param[in]   b
 * @retval      none
 *****************************************************************************/
void DISP_add_back(uint8_t r, uint8_t g, uint8_t b);

void DISP_add_point(uint8_t r, uint8_t g, uint8_t b, int pointSize, int pointCG, int pointPosition);

static inline void DISP_update(void){WS2812B_update();}

void DISP_rgb2hsv(tColor *pRGB, tHSV *pHSV);

void DISP_hsv2rgb(tHSV *pHSV, tColor *pRGB);


#endif
