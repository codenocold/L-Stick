#ifndef _VIDEO_H_
#define _VIDEO_H_


#include "nrf.h"
#include "led.h"


void VIDEO_cylon(void);
void VIDEO_fill_from_palette(const tRGB * pal);
void VIDEO_rainbow(uint8_t deltahue);
void VIDEO_confetti(void);
void VIDEO_sinelon(void);
void VIDEO_bpm(void);
void VIDEO_juggle(void);
void VIDEO_fire(void);
void VIDEO_fire_whith_palette(const tRGB * pal);


#endif
