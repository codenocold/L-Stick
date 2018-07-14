#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "systick.h"
#include "key.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();
    KEY_init(SYSTICK_get_tick);

    NRF_LOG_DEBUG("KEY START");

    uint8_t keyIndex;

    while (true){
        keyIndex = KEY_scan();
        if(keyIndex){
            NRF_LOG_DEBUG("KEY: %d", keyIndex);
        }
    }
}
