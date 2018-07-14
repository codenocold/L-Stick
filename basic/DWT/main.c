#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/*****************************************************************************
 * @bref    DWT_init
 * @param   none
 * @retval  none
 *****************************************************************************/
void DWT_init(void)
{
    uint32_t val;

    // Enable the trace and debug blocks (DWT is one of them).
    val = CoreDebug->DEMCR;
    CoreDebug->DEMCR = val | CoreDebug_DEMCR_TRCENA_Msk;

    // Make the cycle counter is enabled.
    val = DWT->CTRL;
    DWT->CTRL = val | DWT_CTRL_CYCCNTENA_Msk;
}

/*****************************************************************************
 * @bref    DWT_init
 * @param   none
 * @retval  none
 *****************************************************************************/
static inline uint32_t DWT_get_tick(void){return DWT->CYCCNT;}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    // Init DWT
    DWT_init();

    while (true){
        nrf_delay_ms(1000);

        NRF_LOG_DEBUG("CYCCNT: %u", DWT_get_tick());
    }
}
