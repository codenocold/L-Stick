#ifndef _STATE_CTRL_H_
#define _STATE_CTRL_H_


#include "nrf.h"


typedef enum eState{
	STATE_NULL,
    STATE_POWER_UP,
    STATE_POWER_DOWN,
    STATE_POWER_OFF_CHARGING,
    STATE_POWER_OFF_CHARG_COMPLETE,
    STATE_POWER_ON_CHARG,
    STATE_POWER_ON_DISCHARG,
    STATE_LIGHT_MODE_INDICATE,
    STATE_LIGHT_MODE,
}tState;

void STATE_CTRL_loop(void);
tState STATE_CTRL_get_state(void);
void STATE_CTRL_set_state(tState newState);


#endif
