#ifndef _POWER_H_
#define _POWER_H_


#include "nrf.h"
#include "nrf_gpio.h"


#define POWER_LOCK_PIN  6
#define CHRG_STATE_PIN  3
#define CHRG_STDBY_PIN  4


extern volatile uint16_t gBatteryMilivolt;


typedef enum eChargState{
	CHARG_STATE_NO_CHARGER,
	CHARG_STATE_CHARGING,
	CHARG_STATE_CHARG_COMPLETE,
}tChargState;


void POWER_init(void);
__STATIC_INLINE void POWER_lock(void){nrf_gpio_pin_set(POWER_LOCK_PIN);}
__STATIC_INLINE void POWER_unlock(void){nrf_gpio_pin_clear(POWER_LOCK_PIN);}
__STATIC_INLINE tChargState POWER_get_charg_state(void){
	tChargState state;
	if(nrf_gpio_pin_read(CHRG_STATE_PIN) && nrf_gpio_pin_read(CHRG_STDBY_PIN)){
	    // No Charger
	    state = CHARG_STATE_NO_CHARGER;
	}else if(!nrf_gpio_pin_read(CHRG_STATE_PIN) && nrf_gpio_pin_read(CHRG_STDBY_PIN)){
	    // Charging
	    state = CHARG_STATE_CHARGING;
	}else if(nrf_gpio_pin_read(CHRG_STATE_PIN) && !nrf_gpio_pin_read(CHRG_STDBY_PIN)){
	    // Charging Complete
	    state = CHARG_STATE_CHARG_COMPLETE;
	}
	return state;
}


#endif
