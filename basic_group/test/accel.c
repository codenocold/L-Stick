#include "accel.h"
#include "nrf_twi_mngr.h"
#include "lis3dh_driver.h"
#include "nrf_delay.h"
#include <stdbool.h>


//----------------------------------------------------------------------------------
// Varable Definitions
NRF_TWI_MNGR_DEF(m_nrf_twi_mngr, 5, 0);

static const nrf_drv_twi_config_t twi_mngr_config = {
       .scl                = ACCEL_SCL_PIN,
       .sda                = ACCEL_SDA_PIN,
       .frequency          = NRF_TWIM_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_LOWEST,
       .clear_bus_init     = false,
       .hold_bus_uninit    = false
    };

volatile int16_t gAccelRaw_x, gAccelRaw_y, gAccelRaw_z;


//----------------------------------------------------------------------------------
// Function Declarations
static void twi_init(void);
static void twi_callback(ret_code_t result, void * p_user_data);
static int32_t trig_read(bool isBlock);


/*****************************************************************
 * @ bref       ACCEL_init
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_init(void)
{
    uint8_t val;
    status_t response;

    // Config INT pin
    nrf_gpio_cfg_input(ACCEL_INT1, NRF_GPIO_PIN_NOPULL);
    nrf_gpio_cfg_input(ACCEL_INT2, NRF_GPIO_PIN_NOPULL);

    // Twi init
    twi_init();

    // Wait LIS3DH boot complete
    nrf_delay_ms(10);

    // Check Device ID
    response = LIS3DH_GetWHO_AM_I(&val);
    if(response == MEMS_ERROR){
        return -1;
    }
    if(val != 0x33){
        return -2;
    }

    // Set ODR (turn ON device)
    response = LIS3DH_SetODR(LIS3DH_ODR_200Hz);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Set Fullscale
    response = LIS3DH_SetFullScale(LIS3DH_FULLSCALE_2);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Enable Block Data Update continuous
    response = LIS3DH_SetBDU(MEMS_ENABLE);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Set outdata little endian
    response = LIS3DH_SetBLE(LIS3DH_BLE_LSB);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Disable temperature
    response = LIS3DH_SetTemperature(MEMS_DISABLE);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Disable ADC aux
    response = LIS3DH_SetADCAux(MEMS_DISABLE);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Enable data ready Interrupt on INT pin 1
    response = LIS3DH_SetInt1Pin(LIS3DH_I1_DRDY1_ON_INT1_ENABLE);
    if(response == MEMS_ERROR){
        return -1;
    }
    
    // Set Activity/Inactivity Threshold
    response = LIS3DH_SetActivThreshold(5);     // 80 mg
    if(response == MEMS_ERROR){
        return -1;
    }

    // Set Inactivity Duration
    response = LIS3DH_SetActivDuration(100);    // About 4 s
    if(response == MEMS_ERROR){
        return -1;
    }

    // Enable active Interrupt on INT pin 2
    response = LIS3DH_SetInt2Pin(LIS3DH_I2_ACT_ON_INT2_ENABLE);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Set axis Enable
    response = LIS3DH_SetAxis(LIS3DH_X_ENABLE | LIS3DH_Y_ENABLE | LIS3DH_Z_ENABLE);
    if(response == MEMS_ERROR){
        return -1;
    }

    // Set PowerMode 
    response = LIS3DH_SetMode(LIS3DH_POWER_DOWN);
    if(response == MEMS_ERROR){
        return -1;
    }

    return 0;
}

/*****************************************************************
 * @ bref       ACCEL_power_up
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_power_up(void)
{
    status_t response;

    // Set PowerMode 
    response = LIS3DH_SetMode(LIS3DH_NORMAL);
    if(response == MEMS_ERROR){
        return -1;
    }

    return 0;
}

/*****************************************************************
 * @ bref       ACCEL_power_down
 * @ param      none
 * @ retval     0: Success -1: Error
******************************************************************/
int32_t ACCEL_power_down(void)
{
    status_t response;

    // Wait twi idle
    while(!nrf_twi_mngr_is_idle(&m_nrf_twi_mngr)){}

    // Set PowerMode 
    response = LIS3DH_SetMode(LIS3DH_POWER_DOWN);
    if(response == MEMS_ERROR){
        return -1;
    }

    return 0;
}

/*****************************************************************************
 * @bref    ACCEL_loop
 * @param   none
 * @retval  none
*****************************************************************************/
void ACCEL_loop(void)
{
    // Accel data ready
    if(nrf_gpio_pin_read(ACCEL_INT1)){
        if(nrf_twi_mngr_is_idle(&m_nrf_twi_mngr)){
            trig_read(false);
        }
    }
}

/*****************************************************************************
 * @bref    Function for initializing twi.
 * @param   none
 * @retval  none
*****************************************************************************/
static void twi_init(void)
{
    uint32_t err_code;

    err_code = nrf_twi_mngr_init(&m_nrf_twi_mngr, &twi_mngr_config);
    APP_ERROR_CHECK(err_code);
}

/*****************************************************************************
 * @bref    twi read complete callback
 * @param   none
 * @retval  none
*****************************************************************************/
static void twi_callback(ret_code_t result, void * p_user_data)
{
    uint8_t *pBuf = (uint8_t*)p_user_data;

    gAccelRaw_x = (int16_t)(pBuf[1]<<8 | pBuf[0]) >> 4;
    gAccelRaw_y = (int16_t)(pBuf[3]<<8 | pBuf[2]) >> 4;
    gAccelRaw_z = (int16_t)(pBuf[5]<<8 | pBuf[4]) >> 4;
}

/*****************************************************************************
 * @bref      Function for triger accel read
 * @param[in] isBlock Is block mode
 * @retval   0: Success
            -1: Error
*****************************************************************************/
static int32_t trig_read(bool isBlock)
{
    static uint8_t buf[6];
    static const uint8_t reg = 0x28 | 0x80;   // MSB enables address auto increment

    static const nrf_twi_mngr_transfer_t transfers[] =
    {
        NRF_TWI_MNGR_WRITE(ACCEL_SLAVE_ADDR, &reg, 1, NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ (ACCEL_SLAVE_ADDR, buf, 6, 0)
    };
    static const nrf_twi_mngr_transaction_t NRF_TWI_MNGR_BUFFER_LOC_IND transaction =
    {
        .callback            = twi_callback,
        .p_user_data         = buf,
        .p_transfers         = transfers,
        .number_of_transfers = sizeof(transfers) / sizeof(transfers[0]),
        .p_required_twi_cfg  = NULL,
    };

    if(isBlock){
        if(NRF_SUCCESS != nrf_twi_mngr_perform(&m_nrf_twi_mngr, &twi_mngr_config, transfers, 2, NULL)){
            return (-1);
        }
    }else{
        if(NRF_SUCCESS != nrf_twi_mngr_schedule(&m_nrf_twi_mngr, &transaction)){
            return (-1);
        }
    }

    return (0);
}

/****************************************************************************
 * @bref        Function for write reg value
 * @param[in]   reg: Reg addr
 * @param[in]   val: Value
 * @retval      1: Success
                0: Error
****************************************************************************/
uint8_t ACCEL_write_reg(uint8_t reg, uint8_t val)
{
    ret_code_t ret;
    uint8_t buf[2];

    buf[0] = reg;
    buf[1] = val;

    nrf_twi_mngr_transfer_t transfers[] =
    {
        NRF_TWI_MNGR_WRITE(ACCEL_SLAVE_ADDR, buf, 2, 0),
    };

    ret = nrf_twi_mngr_perform(&m_nrf_twi_mngr, &twi_mngr_config, transfers, 1, NULL);
    if(ret != NRF_SUCCESS){
        return (0);
    }

    return (1);
}

/*****************************************************************************
 * @bref        Function for read reg value
 * @param[in]   reg: Reg addr
 * @param[in]   pVal: Value ptr
 * @retval      1: Success
                0: Error
*****************************************************************************/
uint8_t ACCEL_read_reg(uint8_t reg, uint8_t * pVal)
{
    ret_code_t ret;

    nrf_twi_mngr_transfer_t transfers[] =
    {
        NRF_TWI_MNGR_WRITE(ACCEL_SLAVE_ADDR, &reg, 1, NRF_TWI_MNGR_NO_STOP),
        NRF_TWI_MNGR_READ (ACCEL_SLAVE_ADDR, pVal, 1, 0)
    };

    ret = nrf_twi_mngr_perform(&m_nrf_twi_mngr, &twi_mngr_config, transfers, 2, NULL);
    if(ret != NRF_SUCCESS){
        return (0);
    }

    return (1);
}
