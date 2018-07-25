#include "bike_disp.h"
#include "accel.h"
#include "led.h"
#include "fastled.h"
#include "video.h"
#include "dwt.h"


#define ACCE_THRESHOLD              25
#define VARI_THRESHOLD              300

#define FILTER_BUF_NUM              10
#define VARI_BUFF_NUM               50


typedef enum eBikeSpeed{
    BIKE_SPEED_NORMAL,
    BIKE_SPEED_UP,
    BIKE_SPEED_DOWN,
}tBikeSpeed;

typedef struct eBikeState{
    bool isNew;
    tBikeSpeed Speed;
}tBikeState;


//----------------------------------------------------------------------------------
// Varable Definitions
static tBikeState BikeState = {.isNew = false, .Speed = BIKE_SPEED_NORMAL};
static uint8_t FilterBufIndex = 0;
static int16_t FilterBuf[FILTER_BUF_NUM];


//----------------------------------------------------------------------------------
// Function Declarations
static void update_bike_state(void);
static int16_t filtered_accel(void);
static int32_t variance(int16_t *pbuff, int16_t num);


/*****************************************************************************
 * @bref    BIKE_DISP_0
 * @param   none
 * @retval  none
 *****************************************************************************/
void BIKE_DISP_0(void)
{
    update_bike_state();
    ACCEL_loop();

    switch(BikeState.Speed){
        case BIKE_SPEED_NORMAL:
            VIDEO_sinelon();
            break;

        case BIKE_SPEED_UP:
            if(BikeState.isNew){
                BikeState.isNew = false;
                setLEDs(gLED, LED_NUM, 0, 0, 200);
                LED_show();
            }
            break;

        case BIKE_SPEED_DOWN:
            if(BikeState.isNew){
                BikeState.isNew = false;

                setLEDs(gLED, LED_NUM, 255, 0, 0);
                LED_show();
            }
            break;

        default:
            break;
    }
}

static void update_bike_state(void)
{
    static int16_t AccelBufIndex = 0;
    static int16_t AccelBuf[VARI_BUFF_NUM];
    static int16_t AccelBias = 0;
    static tBikeSpeed OldSpeed = BIKE_SPEED_NORMAL;
    tBikeSpeed NewSpeed;
    int16_t accel_filtered;

    // 50Hz
    if(!DWT_is_timeout_us(DWT_TICK_BIKE_LOOP, 20000)){
        return;
    }

    accel_filtered = filtered_accel();

    // Calc accel bias
    AccelBuf[AccelBufIndex++] = accel_filtered;
    if(AccelBufIndex >= VARI_BUFF_NUM){
        AccelBufIndex = 0;
        int32_t va = variance(AccelBuf, VARI_BUFF_NUM);
        if(va < VARI_THRESHOLD){
            int sum = 0;
            for(int i=0; i<VARI_BUFF_NUM; i++){
                sum += AccelBuf[i];
            }
            AccelBias = sum / VARI_BUFF_NUM;
        }
    }

    accel_filtered -= AccelBias;

    if(accel_filtered > +ACCE_THRESHOLD){           // Speed up
        NewSpeed = BIKE_SPEED_UP;
    }else if(accel_filtered < -ACCE_THRESHOLD){     // Speed down
        NewSpeed = BIKE_SPEED_DOWN;
    }else{                                          // Speed normal
        NewSpeed = BIKE_SPEED_NORMAL;
    }

    if(NewSpeed != OldSpeed){
        OldSpeed = NewSpeed;
        BikeState.Speed = NewSpeed;
        BikeState.isNew = true;
    }
}

/**************************************************************************************
 * @bref    filtered_accel
 * @param   none
 * @retval  Result
**************************************************************************************/
static int16_t filtered_accel(void)
{
    int sum = 0;
    int16_t max, min, avg;

    FilterBuf[FilterBufIndex++] = gAccelRaw_z;
    if(FilterBufIndex >= FILTER_BUF_NUM){
        FilterBufIndex = 0;
    }

    max = FilterBuf[0];   
    min = FilterBuf[0];
    for(int i=0; i<FILTER_BUF_NUM; i++){
        if(FilterBuf[i] > max){
            max = FilterBuf[i];
        }
        if(FilterBuf[i] < min){
            min = FilterBuf[i];
        }
        sum += FilterBuf[i];
    }
    avg = (sum - max - min) / (FILTER_BUF_NUM - 2);

    return avg;
}

/**************************************************************************************
 * @bref        variance
 * @param[in]   pbuff: Variance buffer ptr
 * @param[in]   num: Number
 * @retval      Result
**************************************************************************************/
static int32_t variance(int16_t *pbuff, int16_t num)
{
    int32_t i;
    int32_t avr = 0;
    int32_t ret = 0;
    
    for(i=0; i< num; i++){
        avr += pbuff[i];
    }
    
    avr /= num;
    
    for(i=0; i<num; i++){
        ret += (pbuff[i] - avr) * (pbuff[i] - avr);
    }

    ret /= num;
    
    return ret;
}
