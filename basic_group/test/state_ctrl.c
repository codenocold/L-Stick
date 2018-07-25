#include "state_ctrl.h"
#include "storage.h"
#include "power.h"
#include "dwt.h"
#include "fastled.h"
#include "light_mode.h"
#include "nrf_log.h"


//----------------------------------------------------------------------------------
// Varable Definitions
static tState CurrState = STATE_NULL;
static uint32_t StateCtrlStep;


//----------------------------------------------------------------------------------
// Function Declarations
static void power_up(void);
static void power_down(void);
static void light_mode_indicate(void);
static void power_on_charg_connect(void);
static void power_on_charg_disconnect(void);
static void power_off_charging(void);
static void power_off_charg_complete(void);


void STATE_CTRL_loop(void)
{
	switch(CurrState){
        case STATE_NULL:

            break;

		case STATE_POWER_UP:
            power_up();
			break;

    	case STATE_POWER_DOWN:
            power_down();
    		break;

    	case STATE_LIGHT_MODE_INDICATE:
            light_mode_indicate();
    		break;

    	case STATE_LIGHT_MODE:
            LIGHT_MODE_loop();
    		break;

        case STATE_POWER_ON_CHARG:
            power_on_charg_connect();
            break;

        case STATE_POWER_ON_DISCHARG:

            break;

        case STATE_POWER_OFF_CHARGING: 

            break;

        case STATE_POWER_OFF_CHARG_COMPLETE:

            break;

    	default:
    		break;
	}
}

void STATE_CTRL_set_state(tState newState)
{
    StateCtrlStep = 0;
    DWT_is_timeout_us(1, 0);
    CurrState = newState;
}

void power_up(void)
{
    switch(StateCtrlStep){
        case 0:
            POWER_lock();

            // Battery level indicate
            uint8_t num = LED_NUM * POWER_battery_level() / 100;
            if(num == 0){num ++;}
            for(int i=0; i<num; i++){
                gLED[i].R = 0;
                gLED[i].G = 255;
                gLED[i].B = 0;
            }

            StateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(1, 10000)){
                return;
            }

            fadeToBlackBy(gLED, LED_NUM, 20);
            LED_show();
            if(gLED[0].G == 0){
                StateCtrlStep ++;
            }
            break;

        case 2:
            STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
            break;

        default:
            break;
    }
}

static void power_down(void)
{
    switch(StateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 255, 255, 255);
            LED_show();
            StateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(1, 10000)){
                return;
            }

            fadeToBlackBy(gLED, LED_NUM, 20);
            LED_show();
            if(gLED[0].G == 0){
                StateCtrlStep ++;
            }
            break;

        case 2:
            POWER_unlock();
            break;

        default:
            break;
    }
}

static void light_mode_indicate(void)
{
    switch(StateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            setLEDs(gLED, LIGHT_MODE_get_index()+1, 0, 0, 50);
            LED_show();
            StateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(1, 500000)){
                return;
            }
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            StateCtrlStep ++;
            break;

        case 2:
            STATE_CTRL_set_state(STATE_LIGHT_MODE);
            break;

        default:
            break;
    }
}

static void power_on_charg_connect(void)
{
    switch(StateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            StateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(1, 10000)){
                return;
            }

            int i;
            for(i=0; i<LED_NUM; i++){
                if(gLED[i].G != 255){
                    break;
                }
            }

            if(i < LED_NUM){
                tRGB color = {0, 50, 0};
                rgb_add(&gLED[i], color);
                LED_show();
            }else{
                StateCtrlStep ++;
            }
            break;

        case 2:
            STATE_CTRL_set_state(STATE_LIGHT_MODE);
            break;

        default:
            break;
    }
}

static void power_on_charg_disconnect(void)
{
    switch(StateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 255, 0);
            LED_show();
            StateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(1, 10000)){
                return;
            }

            int i;
            for(i=0; i<LED_NUM; i++){
                if(gLED[i].G != 255){
                    break;
                }
            }

            if(i < LED_NUM){
                tRGB color = {0, 50, 0};
                rgb_add(&gLED[i], color);
                LED_show();
            }else{
                StateCtrlStep ++;
            }
            break;

        case 2:
            STATE_CTRL_set_state(STATE_LIGHT_MODE);
            break;

        default:
            break;
    }
}

static void power_off_charging(void)
{

}

static void power_off_charg_complete(void)
{

}
