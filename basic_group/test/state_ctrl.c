#include "state_ctrl.h"
#include "storage.h"
#include "power.h"
#include "dwt.h"
#include "fastled.h"


//----------------------------------------------------------------------------------
// Varable Definitions
static tState CurrState = STATE_NULL;
static uint32_t StateCtrlStep;


//----------------------------------------------------------------------------------
// Function Declarations
static bool power_up_animat(void);
static bool power_down_animat(void);
static bool power_on_charg_animat(void);
static bool power_on_discharg_animat(void);
static void power_off_charg_animat(void);
static void power_off_charg_complete(void);
static bool light_mode_indicate_animat(void);


void STATE_CTRL_loop(void)
{
	switch(CurrState){
        case STATE_NULL:

            break;

		case STATE_POWER_UP:
            if(false == power_up_animat()){
                STORAGE_init();
                STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
            }
			break;

    	case STATE_POWER_DOWN:
            if(false == power_down_animat()){
                if(CHARG_STATE_NO_CHARGER == POWER_get_charg_state()){
                    STATE_CTRL_set_state(STATE_NULL);
                }else{
                    STATE_CTRL_set_state(STATE_POWER_OFF_CHARGING);
                }
                STORAGE_deinit();
                POWER_unlock();
            }
    		break;

        case STATE_POWER_OFF_CHARGING: 
            power_off_charg_animat();
            break;

        case STATE_POWER_OFF_CHARG_COMPLETE:
            power_off_charg_complete();
            break;

        case STATE_POWER_ON_CHARG:
            if(false == power_on_charg_animat()){
                STATE_CTRL_set_state(STATE_LIGHT_MODE);
            }
            break;

        case STATE_POWER_ON_DISCHARG:
            if(false == power_on_discharg_animat()){
                STATE_CTRL_set_state(STATE_LIGHT_MODE);
            }
            break;

    	case STATE_LIGHT_MODE_INDICATE:
            if(false == light_mode_indicate_animat()){
                STATE_CTRL_set_state(STATE_LIGHT_MODE);
            }
    		break;

    	case STATE_LIGHT_MODE:

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

static bool power_up_animat(void)
{
    if(!DWT_is_timeout_us(1, 10000)){
        return true;
    }

    if(StateCtrlStep == 0){
        StateCtrlStep ++;
        tRGB color = {255, 255, 255};
        for(int i=0; i<LED_NUM; i++){
            gLED[i] = color;
        }
    }else{
        fadeToBlackBy(gLED, LED_NUM, 10);
        LED_show();
    }

    return true;
}

static bool power_down_animat(void)
{
    
    return true;
}

static bool power_on_charg_animat(void)
{

    return true;
}

static bool power_on_discharg_animat(void)
{

    return true;
}

static void power_off_charg_animat(void)
{

}

static void power_off_charg_complete(void)
{
   
}

static bool light_mode_indicate_animat(void)
{
    return true;
}
