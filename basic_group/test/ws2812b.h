#ifndef _WS2812B_H_
#define _WS2812B_H_


#include "nrfx_pwm.h"


#define WS2812B_PIN	    	   11
#define WS2812B_NUMLEDS        16
#define WS2812B_BITS_PER_LED   24


typedef struct sRGB{
    uint8_t R;		// Red
    uint8_t G;		// Green
    uint8_t B;		// Blue
}tRGB;

typedef struct sHSV{
	uint8_t H;		// Hue
	uint8_t S;		// Saturation
	uint8_t V;		// Value
}tHSV;


/*****************************************************************************
 * @bref        Init ws2812b driver
 * @param[in]   none
 * @retval      none
 *****************************************************************************/
void WS2812B_init(void);

/*****************************************************************************
 * @bref        Set a led color
 * @param[in]   pRGB led color ptr
 * @param[in]   led led number
 * @param[in]   update Perform an update after changing led color
 * @retval      none
 *****************************************************************************/
void WS2812B_setColorRGB(tRGB* pRGB, uint8_t led, bool update);

/*****************************************************************************
 * @bref        Get a led color
 * @param[in]   led led number
 * @retval      tRGB
 *****************************************************************************/
tRGB WS2812B_getColorRGB(uint8_t led);

/*****************************************************************************
 * @bref    Update led color
 * @param   none
 * @retval  none
 *****************************************************************************/
void WS2812B_update(void);


#endif
