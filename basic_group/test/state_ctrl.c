#include "state_ctrl.h"
#include "storage.h"
#include "power.h"
#include "dwt.h"
#include "fastled.h"
#include "light_mode.h"
#include "accel.h"


//----------------------------------------------------------------------------------
// Varable Definitions
static tState mCurrState = STATE_NULL;
static uint32_t mStateCtrlStep;


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
	switch(mCurrState){
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

        case STATE_ON_CHARG_CONNECT:
            power_on_charg_connect();
            break;

        case STATE_ON_CHARG_DISCONNECT:
            power_on_charg_disconnect();
            break;

        case STATE_OFF_CHARGING:
            power_off_charging();
            break;

        case STATE_OFF_CHARG_COMPLETE:
            power_off_charg_complete();
            break;

    	default:
    		break;
	}

    // Charger loop ctrl
    static tChargState ChargState = CHARG_STATE_NULL;
    if(DWT_is_timeout_us(DWT_TICK_CHARGER_LOOP, 500000)){
        if(ChargState == CHARG_STATE_NULL){
            ChargState = POWER_get_charg_state();
        }else{
            if(ChargState != POWER_get_charg_state()){
                ChargState = POWER_get_charg_state();

                if(ChargState == CHARG_STATE_NO_CHARGER){
                    if(POWER_is_locked()){
                        STATE_CTRL_set_state(STATE_ON_CHARG_DISCONNECT);
                    }
                }else if(ChargState == CHARG_STATE_CHARGING){
                    if(POWER_is_locked()){
                        STATE_CTRL_set_state(STATE_ON_CHARG_CONNECT);
                    }else{
                        STATE_CTRL_set_state(STATE_OFF_CHARGING);
                    }
                }else if(ChargState == CHARG_STATE_CHARG_COMPLETE){
                    if(POWER_is_locked()){
                        STATE_CTRL_set_state(STATE_ON_CHARG_CONNECT);
                    }else{
                        STATE_CTRL_set_state(STATE_OFF_CHARG_COMPLETE);
                    }
                }
            }
        }
    }

    // Accel on/off loop ctrl
    if(DWT_is_timeout_us(DWT_TICK_ACCEL_SWITCH_LOOP, 100000)){
        if(gAccelEnableCnt > 0 && !ACCEL_is_enable()){
            ACCEL_power_up();
        }
        if(gAccelEnableCnt <= 0 && ACCEL_is_enable()){
            ACCEL_power_down();
        }
    }
}

void STATE_CTRL_set_state(tState newState)
{
    mStateCtrlStep = 0;
    DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 0);
    mCurrState = newState;
}

void power_up(void)
{
    switch(mStateCtrlStep){
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

            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            fadeToBlackBy(gLED, LED_NUM, 20);
            LED_show();
            if(gLED[0].G == 0){
                mStateCtrlStep ++;
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
    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 255, 255, 255);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            fadeToBlackBy(gLED, LED_NUM, 20);
            LED_show();
            if(gLED[0].G == 0){
                mStateCtrlStep ++;
            }
            break;

        case 2:
            POWER_unlock();

            if(POWER_get_charg_state() == CHARG_STATE_CHARGING){
                STATE_CTRL_set_state(STATE_OFF_CHARGING);
            }else if(POWER_get_charg_state() == CHARG_STATE_CHARG_COMPLETE){
                STATE_CTRL_set_state(STATE_OFF_CHARG_COMPLETE);
            }
            break;

        default:
            break;
    }
}

static void light_mode_indicate(void)
{
    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            setLEDs(gLED, G_ModeIndex+1, 0, 0, 50);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 500000)){
                return;
            }
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            mStateCtrlStep ++;
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
    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
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
                mStateCtrlStep ++;
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
    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 255, 0);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            int i;
            for(i=LED_NUM-1; i>=0; i--){
                if(gLED[i].G != 0){
                    break;
                }
            }

            if(i >= 0){
                tRGB color = {0, 50, 0};
                rgb_sub(&gLED[i], color);
                LED_show();
            }else{
                mStateCtrlStep ++;
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
    tRGB color = {0, 1, 0};

    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            
            rgb_add(&gLED[0], color);
            LED_show();

            if(gLED[0].G == 255){
                mStateCtrlStep ++;
            }
            break;

        case 2:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            rgb_sub(&gLED[0], color);
            LED_show();

            if(gLED[0].G < 10){
                mStateCtrlStep = 1;
            }
            break;

        default:
            break;
    }
}

static void power_off_charg_complete(void)
{
    tRGB color = {1, 0, 0};

    switch(mStateCtrlStep){
        case 0:
            setLEDs(gLED, LED_NUM, 0, 0, 0);
            LED_show();
            mStateCtrlStep ++;
            break;

        case 1:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            rgb_add(&gLED[0], color);
            LED_show();

            if(gLED[0].R == 255){
                mStateCtrlStep ++;
            }
            break;

        case 2:
            if(!DWT_is_timeout_us(DWT_TICK_MAIN_LOOP, 10000)){
                return;
            }

            rgb_sub(&gLED[0], color);
            LED_show();

            if(gLED[0].R < 10){
                mStateCtrlStep = 1;
            }
            break;

        default:
            break;
    }
}
