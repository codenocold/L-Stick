#include "nrf_delay.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "systick.h"
#include "led.h"
#include "video.h"
#include "accel.h"
#include "dwt.h"
#include "key.h"
#include "power.h"
#include "storage.h"


int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    ACCEL_init();
    DWT_init();
    SYSTICK_init();
    LED_init();
    KEY_init(SYSTICK_get_tick);
    POWER_init();
    STORAGE_init();

    ACCEL_power_up();

    NRF_LOG_INFO("mode %d", G_ModeIndex);

    while (true){
        // VIDEO_cylon();
        // VIDEO_fill_from_palette(PaletteRainbowStripeColors_p);
    	// VIDEO_rainbow(7);
    	// VIDEO_confetti();
    	// VIDEO_sinelon();
    	// VIDEO_bpm();
    	// VIDEO_juggle();
    	// VIDEO_fire();
    	// VIDEO_fire_whith_palette(PaletteHeatColors_p);

        uint8_t key = KEY_scan();
        if(key){
            NRF_LOG_INFO("key %d", key);
            G_ModeIndex = key;
            STORAGE_deinit();
        }
    }
}
