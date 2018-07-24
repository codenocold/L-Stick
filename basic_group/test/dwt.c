#include "dwt.h"


//----------------------------------------------------------------------------------
// Varable Definitions


//----------------------------------------------------------------------------------
// Function Declarations


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
