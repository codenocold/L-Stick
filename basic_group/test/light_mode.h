#ifndef _LIGHT_MODE_H_
#define _LIGHT_MODE_H_


#include "nrf.h"


typedef enum eLightMode{
    LM_STATIC_LIGHT_0 = 0,
    LM_STATIC_LIGHT_1,
    LM_STATIC_LIGHT_2,
    LM_STATIC_LIGHT_3,
    LM_STATIC_LIGHT_4,
    LM_STATIC_LIGHT_5,
    LM_IMG_DISP_0,
    LM_IMG_DISP_1,
    LM_IMG_DISP_2,
    LM_IMG_DISP_3,
    LM_IMG_DISP_4,
    LM_IMG_DISP_5,

    LM_INTERACT_0,
    LM_INTERACT_1,
    LM_INTERACT_2,
    LM_INTERACT_3,
    LM_INTERACT_4,

    LM_BIKE_0,
    LM_BIKE_1,
    LM_BIKE_2,
    LM_BIKE_3,
    LM_BIKE_4,

    LM_VIDEO_0,
    LM_VIDEO_1,
    LM_VIDEO_2,
    LM_VIDEO_3,
    LM_VIDEO_4,
    LM_VIDEO_5,
    LM_VIDEO_6,
    LM_VIDEO_7,
    LM_VIDEO_8,
    LM_VIDEO_9,
    LM_VIDEO_10,
    LM_VIDEO_11,
    LM_VIDEO_12,
    LM_VIDEO_13,
    LM_VIDEO_14,
    LM_VIDEO_15,
    LM_VIDEO_16,
    LM_VIDEO_17,
    LM_VIDEO_18,
    LM_VIDEO_19,
    LM_VIDEO_20,

    LM_NULL = 0xFF,
}tLightMode;


void LIGHT_MODE_index_prev(void);
void LIGHT_MODE_index_next(void);
void LIGHT_MODE_loop(void);


#endif
