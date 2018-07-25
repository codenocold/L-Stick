#include "light_mode.h"
#include "state_ctrl.h"
#include "storage.h"
#include "video.h"
#include "fastled.h"


//----------------------------------------------------------------------------------
// Varable Definitions


//----------------------------------------------------------------------------------
// Function Declarations


void LIGHT_MODE_set_index(int32_t index)
{
    G_ModeIndex = index;
    STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
}

int32_t LIGHT_MODE_get_index(void)
{
    return G_ModeIndex;
}

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
    
    STATE_CTRL_set_state(STATE_LIGHT_MODE_INDICATE);
}

void LIGHT_MODE_loop(void)
{
    switch(G_pModeTable->table[G_ModeIndex]){
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
