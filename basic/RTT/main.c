#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    uint8_t bytes[] = {0x12, 0x34, 0xAC, 0xBE};

    while (true){
        nrf_delay_ms(1000);

        NRF_LOG_DEBUG("Hello World");
        NRF_LOG_INFO("Hello World");
        NRF_LOG_WARNING("Hello World");
        NRF_LOG_ERROR("Hello World");

        NRF_LOG_HEXDUMP_DEBUG(bytes, sizeof(bytes));
        NRF_LOG_HEXDUMP_INFO(bytes, sizeof(bytes));
        NRF_LOG_HEXDUMP_WARNING(bytes, sizeof(bytes));
        NRF_LOG_HEXDUMP_ERROR(bytes, sizeof(bytes));
    }
}
