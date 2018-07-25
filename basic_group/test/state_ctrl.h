#ifndef _STATE_CTRL_H_
#define _STATE_CTRL_H_


#include "nrf.h"


typedef enum eState{
	STATE_NULL,
    STATE_POWER_UP,
    STATE_POWER_DOWN,
    STATE_LIGHT_MODE_INDICATE,
    STATE_LIGHT_MODE,
    STATE_ON_CHARG_CONNECT,
    STATE_ON_CHARG_DISCONNECT,
    STATE_OFF_CHARGING,
    STATE_OFF_CHARG_COMPLETE,
}tState;


void STATE_CTRL_loop(void);
void STATE_CTRL_set_state(tState newState);


#endif
