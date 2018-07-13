#include "systick.h"


//----------------------------------------------------------------------------------
// Varable Definitions
volatile uint32_t G_Tick = 0;


//----------------------------------------------------------------------------------
// Function Declarations


/*****************************************************************************
 * @bref    SYSTICK_init
 * @param   none
 * @retval  none
 *****************************************************************************/
void SYSTICK_init(void)
{
    NVIC_SetPriority(SysTick_IRQn, APP_IRQ_PRIORITY_LOWEST);

    nrf_systick_load_set(64000);

    nrf_systick_csr_set(
        NRF_SYSTICK_CSR_CLKSOURCE_CPU |
        NRF_SYSTICK_CSR_TICKINT_ENABLE |
        NRF_SYSTICK_CSR_ENABLE);
}

/*****************************************************************************
 * @bref   SysTick_Handler Callback
 * @param  none
 * @retval none
 *****************************************************************************/
void SysTick_Handler(void)
{
    G_Tick ++;
}
