#ifndef _VIDEO_H_
#define _VIDEO_H_


#include "nrf.h"
#include "led.h"
#include "fastled.h"
#include "systick.h"


void VIDEO_cylon(void);
void VIDEO_fill_from_palette(const tRGB * pal);


#endif
