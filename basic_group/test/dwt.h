#ifndef _DWT_H_
#define _DWT_H_


#include "nrf.h"


void DWT_init(void);
__STATIC_INLINE uint32_t DWT_get_tick(void){return DWT->CYCCNT;}
__STATIC_INLINE uint32_t DWT_get_us_since(uint32_t t){return (DWT->CYCCNT - t);}


#endif
