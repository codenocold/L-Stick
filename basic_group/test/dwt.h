#ifndef _DWT_H_
#define _DWT_H_


#include "nrf.h"
#include <stdbool.h>


#define DWT_TICK_NUM	3


extern uint32_t gDwtTickArray[DWT_TICK_NUM];


void DWT_init(void);
__STATIC_INLINE uint32_t DWT_get_tick(void){return DWT->CYCCNT;}
__STATIC_INLINE uint32_t DWT_get_us_since(uint32_t t){return (DWT->CYCCNT - t)/64;}

__STATIC_INLINE bool DWT_is_timeout_tick(uint8_t index, uint32_t tick){
	if(tick == 0){
		gDwtTickArray[index] = DWT_get_tick();
	}else if((uint32_t)(DWT->CYCCNT - gDwtTickArray[index]) > tick){
		gDwtTickArray[index] = DWT_get_tick();
		return true;
	}
	return false;
}

__STATIC_INLINE bool DWT_is_timeout_us(uint8_t index, uint32_t us){
	if(us == 0){
		gDwtTickArray[index] = DWT_get_tick();
	}else if(DWT_get_us_since(gDwtTickArray[index]) > us){
		gDwtTickArray[index] = DWT_get_tick();
		return true;
	}
	return false;
}


#endif
