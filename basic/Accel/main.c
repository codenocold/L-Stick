#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "accel.h"


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    int32_t ret;

    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    ret = ACCEL_init();
    if(ret != 0){
    	NRF_LOG_ERROR("ACCEL_init fail %d", ret);
    }else{
    	NRF_LOG_INFO("ACCEL_init ok");
    }

    bool isActiv = false;

    while (true){
        ACCEL_loop();

        if(ACCEL_is_activ()){
            if(!isActiv){
                isActiv = true;
                NRF_LOG_INFO("Activ");
            }
        }else{
            if(isActiv){
                isActiv = false;
                NRF_LOG_INFO("Inactiv");
            }
        }
    }
}
