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
#include "state_ctrl.h"


static void key_loop(void);


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

        key_loop();
    }
}


static void key_loop(void)
{
    switch(KEY_scan()){
        case 1:     // Click
            if(POWER_is_locked()){
                //LIGHT_MODE_index_next();
            }
            break;

        case 2:     // Double click
            if(POWER_is_locked()){
                //LIGHT_MODE_index_prev();
            }
            break;

        case 3:     // Triple click

            break;

        case 255:   // Long press
            if(POWER_is_locked()){
                // Power off
                STATE_CTRL_set_state(STATE_POWER_DOWN);
            }else{
                // Power on
                POWER_lock();
                STATE_CTRL_set_state(STATE_POWER_UP);
            }
            break;

        default:
            break;
    }
}
