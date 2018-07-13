#ifndef __LIS3DH_H_
#define __LIS3DH_H_


#include "nrf.h"
#include "nrf_gpio.h"
#include <stdbool.h>


#define ACCEL_SLAVE_ADDR          0x19

#define ACCEL_INT1  			  25
#define ACCEL_INT2  			  26
#define ACCEL_SCL_PIN             28    // SCL signal pin
#define ACCEL_SDA_PIN             27    // SDA signal pin


/*****************************************************************
 * @ bref       ACCEL_init
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_init(void);

/*****************************************************************
 * @ bref       ACCEL_power_up
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_power_up(void);

/*****************************************************************
 * @ bref       ACCEL_power_down
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_power_down(void);

/*****************************************************************************
 * @bref    ACCEL_loop
 * @param   none
 * @retval  none
*****************************************************************************/
void ACCEL_loop(void);

/*****************************************************************************
 * @bref    ACCEL_is_activ
 * @param   none
 * @retval  Accel activ state
*****************************************************************************/
static inline int32_t ACCEL_is_activ(void){return !nrf_gpio_pin_read(ACCEL_INT2);}

/****************************************************************************
 * @bref        Function for write reg value
 * @param[in]   reg: Reg addr
 * @param[in]   val: Value
 * @retval      1: Success
                0: Error
****************************************************************************/
uint8_t ACCEL_write_reg(uint8_t reg, uint8_t val);

/*****************************************************************************
 * @bref        Function for read reg value
 * @param[in]   reg: Reg addr
 * @param[in]   pVal: Value ptr
 * @retval      1: Success
                0: Error
*****************************************************************************/
uint8_t ACCEL_read_reg(uint8_t reg, uint8_t * pVal);


#endif