#ifndef _KEY_H_
#define _KEY_H_


#include "nrf.h"
#include "nrf_gpio.h"


#define KEY_PIN     (7)


typedef uint32_t (*p_get_key_timestamp_func)(void);


/*****************************************************************************
 * @bref        KEY_init
 * @param[in]   pfunc Get tick func ptr
 * @retval      none
 *****************************************************************************/
void KEY_init(p_get_key_timestamp_func pfunc);

/*****************************************************************************
 * @bref   KEY_scan
 * @param  none
 * @retval Key index
 *****************************************************************************/
uint8_t KEY_scan(void);

/*****************************************************************************
 * @bref   KEY_is_press
 * @param  none
 * @retval 0 if the key is not pressed
 *****************************************************************************/
__STATIC_INLINE uint32_t KEY_is_press(void){return nrf_gpio_pin_read(KEY_PIN);}


#endif
