#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "systick.h"
#include "led.h"
#include "video.h"


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    SYSTICK_init();
    LED_init();

    while (true){
        // VIDEO_cylon();
        // VIDEO_fill_from_palette(PaletteRainbowColors_p);
    }
}
