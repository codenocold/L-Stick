#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "systick.h"
#include "led.h"


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();

    LED_init();

    for(int i=0; i<LED_NUM; i++){
        gLED[i].R = 0;
        gLED[i].G = 200;
        gLED[i].B = 200;
    }

    LED_update();
    LED_show();
    
    while (true){

        nrf_delay_ms(16);
    }
}
