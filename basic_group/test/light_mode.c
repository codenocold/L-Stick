#include "light_mode.h"
#include "state_ctrl.h"
#include "storage.h"
#include "video.h"
#include "fastled.h"
#include "bike_disp.h"
#include "accel.h"


//----------------------------------------------------------------------------------
// Varable Definitions
static uint32_t mLightModeStep;


//----------------------------------------------------------------------------------
// Function Declarations


void LIGHT_MODE_index_prev(void)
{
    if(G_ModeIndex == 0){
        for(uint8_t i=15; i>0; i--){
            if(G_pModeTable->table[i] != (uint8_t)0xFF){
                G_ModeIndex = i;
                break;
            }
        }
    }else{
        G_ModeIndex --;
    }

    mLightModeStep = 0;
    STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
}

void LIGHT_MODE_index_next(void)
{
    if(G_ModeIndex == 15){
        G_ModeIndex = 0;
    }else{
        G_ModeIndex ++;
        if(G_pModeTable->table[G_ModeIndex] == 0xFF){
            G_ModeIndex = 0;
        }
    }
    
    mLightModeStep = 0;
    STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
}

void LIGHT_MODE_loop(void)
{
    static bool isUseAccel = false;
    static tLightMode LastLightMode = LM_NULL;

    // Light mode change event
    if(G_pModeTable->table[G_ModeIndex] != LastLightMode){
        LastLightMode = G_pModeTable->table[G_ModeIndex];
        
        if(isUseAccel){
            isUseAccel = false;
            gAccelEnableCnt --;
        }
    }

    switch(G_pModeTable->table[G_ModeIndex]){
        case LM_STATIC_LIGHT_0:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[0].R, G_pStaticLight->color[0].G, G_pStaticLight->color[0].B);
                LED_show();
            }
            break;

        case LM_STATIC_LIGHT_1:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[1].R, G_pStaticLight->color[1].G, G_pStaticLight->color[1].B);
                LED_show();
            }
            break;

        case LM_STATIC_LIGHT_2:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[2].R, G_pStaticLight->color[2].G, G_pStaticLight->color[2].B);
                LED_show();
            }
            break;

        case LM_STATIC_LIGHT_3:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[3].R, G_pStaticLight->color[3].G, G_pStaticLight->color[3].B);
                LED_show();
            }
            break;

        case LM_STATIC_LIGHT_4:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[4].R, G_pStaticLight->color[4].G, G_pStaticLight->color[4].B);
                LED_show();
            }
            break;

        case LM_STATIC_LIGHT_5:
            if(mLightModeStep == 0){
                mLightModeStep ++;
                setLEDs(gLED, LED_NUM, G_pStaticLight->color[5].R, G_pStaticLight->color[5].G, G_pStaticLight->color[5].B);
                LED_show();
            }
            break;

        case LM_BIKE_0:
            if(mLightModeStep == 0){
                mLightModeStep ++;

                // Enable Accel
                isUseAccel = true;
                gAccelEnableCnt ++;
            }
            BIKE_DISP_0();
            break;

        case LM_VIDEO_0:
            VIDEO_cylon();
            break;

        case LM_VIDEO_1:
            VIDEO_fill_from_palette(PaletteRainbowStripeColors_p);
            break;

        case LM_VIDEO_2:
            VIDEO_rainbow(7);
            break;

        case LM_VIDEO_3:
            VIDEO_confetti();
            break;

        case LM_VIDEO_4:
            VIDEO_sinelon();
            break;

        case LM_VIDEO_5:
            VIDEO_bpm();
            break;

        case LM_VIDEO_6:
            VIDEO_juggle();
            break;

        case LM_VIDEO_7:
            VIDEO_fire();
            break;

        case LM_VIDEO_8:
            VIDEO_fire_whith_palette(PaletteHeatColors_p);
            break;

        default:
            break;
    }
}
