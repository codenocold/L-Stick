#include "disp.h"


//----------------------------------------------------------------------------------
// Varable Definitions


//----------------------------------------------------------------------------------
// Function Declarations


/*****************************************************************************
 * @bref        Function for add back to the buf.
 * @param[in]   hsv
 * @retval      none
 *****************************************************************************/
void DISP_add_back(tHSV hsv)
{
    tRGB rgb;

    DISP_hsv2rgb(&hsv, &rgb);

    for(int i=0; i<WS2812B_NUMLEDS; i++){
        WS2812B_setColorRGB(&rgb, i, false);
    }
}

/*****************************************************************************
 * @bref        Function for add point to the buf.
 * @param[in]   hsv
 * @param[in]   pointSize     (0~15000)
 * @param[in]   pointCG       (0~1000 )
 * @param[in]   pointPosition (0~15000) LE0~LED15
 * @retval      none
 *****************************************************************************/
void DISP_add_point(tHSV hsv, int pointSize, int pointCG, int pointPosition)
{
    tRGB rgb;
    tHSV hsv_tmp = hsv;
    int index;
    int sub;

    int start;
    int end;

    start = (pointPosition - pointSize/2);
    index = start / 1000;
    sub = start % 1000;
    sub = 1000 - sub;
    hsv_tmp.V = sub * 
    if(sub != 0){
        start = hsv.V - pointSize * sub/1000;
        end   = hsv.V - pointSize * (1000-sub)/1000;
    }

    hsv_tmp.V = start;
    for(int i = index; i>=0; i--){
        DISP_hsv2rgb(&hsv_tmp, &rgb);
        WS2812B_setColorRGB(&rgb, i, false);
        if(hsv_tmp.V > pointPosition){
            hsv_tmp.V -= pointPosition;
        }else{
            break;
        }
    }

    hsv_tmp.V = end;
    for(int i = index; i<16; i++){
        DISP_hsv2rgb(&hsv_tmp, &rgb);
        WS2812B_setColorRGB(&rgb, i, false);
        if(hsv_tmp.V > pointPosition){
            hsv_tmp.V -= pointPosition;
        }else{
            break;
        }
    }
}

{
    int pose;
    int spl;
    int val;
    int C1,C2,B;
    int HalfPointSize;
    tColor color;

    pointCG = pointSize * pointCG / 1000;
    HalfPointSize = pointSize / 2;

    if(pointPosition - HalfPointSize > 15000){
        return;
    }
    if(pointPosition + HalfPointSize < 0){
        return;
    }

    B  = pointCG;
    C1 = (pointPosition - HalfPointSize) * 1000 / B;

    B  = pointSize - pointCG;
    C2 = -(pointPosition + HalfPointSize) * 1000 / B;

    spl = pointPosition - HalfPointSize + pointCG;

    for(int i=0; i<WS2812B_NUMLEDS; i++){
        pose = i * 1000;
        if(pose < spl){
            val =  (1000 * pose) / pointCG - C1;
        }else if(pose > spl){
            val = -(1000 * pose) / B - C2;
        }else{
            val = 1000;
        }

        if(val > 0){
            color.R = r * val / 1000;
            color.G = g * val / 1000;
            color.B = b * val / 1000;
            WS2812B_setColorRGB(&color, i, false);
        }
    }
}

void DISP_rgb2hsv(tRGB *pRGB, tHSV *pHSV)
{
    uint8_t max, min, delta;

    max = pRGB->R > pRGB->G ? pRGB->R : pRGB->G;
    if(max < pRGB->B){
        max = pRGB->B;
    }

    min = pRGB->R < pRGB->G ? pRGB->R : pRGB->G;
    if(min > pRGB->B){
        min = pRGB->B;
    }

    delta = max - min;

    pHSV->V = max;

    if(delta == 0){      
        pHSV->H = 0; 
        pHSV->S = 0; 
    }else{
        pHSV->S = 255 * delta / max;

        if(pRGB->R == max){
            pHSV->H = 60 * (pRGB->G - pRGB->B) / delta;
        }else if(pRGB->G == max){
            pHSV->H = 120 + 60 * (pRGB->B - pRGB->R) / delta;
        }else{
            pHSV->H = 240 + 60 * (pRGB->R - pRGB->G) / delta;
        }

        if(pHSV->H < 0){
            pHSV->H += 360;
        }
    }
}

void DISP_hsv2rgb(tHSV *pHSV, tRGB *pRGB)  
{
    uint8_t sec;
    uint8_t RGB_min;
    uint8_t RGB_adj;

    sec = pHSV->H / 60;
    RGB_min = pHSV->V * (255 - pHSV->S) / 255;     

    // RGB adjustment amount by hue   
    RGB_adj = (pHSV->V - RGB_min) * (pHSV->H % 60) / 60;  
  
    switch(sec){  
        case 0:  
            pRGB->R = pHSV->V;  
            pRGB->G = RGB_min + RGB_adj;  
            pRGB->B = RGB_min;  
            break;

        case 1:  
            pRGB->R = pHSV->V - RGB_adj;  
            pRGB->G = pHSV->V;  
            pRGB->B = RGB_min;  
            break;

        case 2:  
            pRGB->R = RGB_min;  
            pRGB->G = pHSV->V;  
            pRGB->B = RGB_min + RGB_adj;  
            break;

        case 3:  
            pRGB->R = RGB_min;  
            pRGB->G = pHSV->V - RGB_adj;  
            pRGB->B = pHSV->V;  
            break;

        case 4:  
            pRGB->R = RGB_min + RGB_adj;  
            pRGB->G = RGB_min;  
            pRGB->B = pHSV->V;  
            break;

        default:        // case 5:  
            pRGB->R = pHSV->V;  
            pRGB->G = RGB_min;  
            pRGB->B = pHSV->V - RGB_adj;  
            break;  
    }
}
