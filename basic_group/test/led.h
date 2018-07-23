#ifndef _led_H_
#define _led_H_


#include "nrf.h"
#include "nrfx_pwm.h"


#define LED_NUM		16


typedef struct sRGB{
	uint8_t B;		// Blue
    uint8_t R;		// Red
    uint8_t G;		// Green
}tRGB;

typedef struct sHSV{
	uint8_t H;		// Hue
	uint8_t S;		// Saturation
	uint8_t V;		// Value
}tHSV;


extern tRGB gLED[LED_NUM];


void LED_init(void);
void LED_setColorRGB(tRGB rgb, uint8_t index, bool isShow);
void LED_show(void);


#endif
