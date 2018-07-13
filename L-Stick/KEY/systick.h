#ifndef _SYSTICK_H_
#define _SYSTICK_H_


#include "nrf.h"
#include "nrf_systick.h"


extern volatile uint32_t G_Tick;


/*****************************************************************************
 * @bref    SYSTICK_init
 * @param   none
 * @retval  none
 *****************************************************************************/
void SYSTICK_init(void);

/**************************************************************************************
 * @bref    Query the current tick
 * @param   none
 * @retval  A tick value
**************************************************************************************/
__STATIC_INLINE uint32_t SYSTICK_get_tick(void)
{
    return G_Tick;
}

/**************************************************************************************
 * @bref        Calculate time in milliseconds since a previous time
 * @param[in]   t: previous tick value
 * @retval      time in us since previous time t.
**************************************************************************************/
__STATIC_INLINE uint32_t SYSTICK_get_ms_since(uint32_t t)
{
    return (G_Tick - t);
}


#endif
