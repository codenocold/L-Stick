#include "disp.h"


//----------------------------------------------------------------------------------
// Varable Definitions


//----------------------------------------------------------------------------------
// Function Declarations


// /*****************************************************************************
//  * @bref        Function for add back to the buf.
//  * @param[in]   hsv
//  * @retval      none
//  *****************************************************************************/
// void DISP_add_back(tHSV hsv)
// {
//     tRGB rgb;

//     DISP_hsv2rgb(&hsv, &rgb);

//     for(int i=0; i<WS2812B_NUMLEDS; i++){
//         WS2812B_setColorRGB(&rgb, i, false);
//     }
// }

// void DISP_rgb2hsv(tRGB *pRGB, tHSV *pHSV)
// {
//     uint8_t max, min, delta;

//     max = pRGB->R > pRGB->G ? pRGB->R : pRGB->G;
//     if(max < pRGB->B){
//         max = pRGB->B;
//     }

//     min = pRGB->R < pRGB->G ? pRGB->R : pRGB->G;
//     if(min > pRGB->B){
//         min = pRGB->B;
//     }

//     delta = max - min;

//     pHSV->V = max;

//     if(delta == 0){      
//         pHSV->H = 0; 
//         pHSV->S = 0; 
//     }else{
//         pHSV->S = 255 * delta / max;

//         if(pRGB->R == max){
//             pHSV->H = 60 * (pRGB->G - pRGB->B) / delta;
//         }else if(pRGB->G == max){
//             pHSV->H = 120 + 60 * (pRGB->B - pRGB->R) / delta;
//         }else{
//             pHSV->H = 240 + 60 * (pRGB->R - pRGB->G) / delta;
//         }

//         if(pHSV->H < 0){
//             pHSV->H += 360;
//         }
//     }
// }

// void DISP_hsv2rgb(tHSV *pHSV, tRGB *pRGB)  
// {
//     uint8_t sec;
//     uint8_t RGB_min;
//     uint8_t RGB_adj;
//     uint16_t Hue = pHSV->H * 1.4f;

//     sec = Hue / 60;
//     RGB_min = pHSV->V * (255 - pHSV->S) / 255;     

//     // RGB adjustment amount by hue   
//     RGB_adj = (pHSV->V - RGB_min) * (Hue % 60) / 60;  
  
//     switch(sec){  
//         case 0:  
//             pRGB->R = pHSV->V;  
//             pRGB->G = RGB_min + RGB_adj;  
//             pRGB->B = RGB_min;  
//             break;

//         case 1:  
//             pRGB->R = pHSV->V - RGB_adj;  
//             pRGB->G = pHSV->V;  
//             pRGB->B = RGB_min;  
//             break;

//         case 2:  
//             pRGB->R = RGB_min;  
//             pRGB->G = pHSV->V;  
//             pRGB->B = RGB_min + RGB_adj;  
//             break;

//         case 3:  
//             pRGB->R = RGB_min;  
//             pRGB->G = pHSV->V - RGB_adj;  
//             pRGB->B = pHSV->V;  
//             break;

//         case 4:  
//             pRGB->R = RGB_min + RGB_adj;  
//             pRGB->G = RGB_min;  
//             pRGB->B = pHSV->V;  
//             break;

//         default:        // case 5:  
//             pRGB->R = pHSV->V;  
//             pRGB->G = RGB_min;  
//             pRGB->B = pHSV->V - RGB_adj;  
//             break;  
//     }
// }






tRGB ColorFromPalette( const tRGB *pal, uint8_t index, uint8_t brightness, tBlendType blendType)
{
    uint8_t hi4 = index >> 4;
    uint8_t lo4 = index & 0x0F;
    uint8_t hi4XsizeofCRGB = hi4 * sizeof(tRGB);

    // We then add that to a base array pointer.
    const tRGB* entry = (tRGB*)( (uint8_t*)(&(pal[0])) + hi4XsizeofCRGB);
    
    uint8_t blend = lo4 && (blendType != NOBLEND);
    
    uint8_t red1   = entry->R;
    uint8_t green1 = entry->G;
    uint8_t blue1  = entry->B;
    
    if( blend ) {
        if( hi4 == 15 ) {
            entry = &(pal[0]);
        } else {
            entry++;
        }
        
        uint8_t f2 = lo4 << 4;
        uint8_t f1 = 255 - f2;

        uint8_t red2   = entry->R;
        red1   = scale8( red1, f1);
        red2   = scale8( red2, f2);
        red1   += red2;

        uint8_t green2 = entry->G;
        green1 = scale8( green1, f1);
        green2 = scale8( green2, f2);
        green1 += green2;

        uint8_t blue2  = entry->B;
        blue1  = scale8( blue1, f1);
        blue2  = scale8( blue2, f2);
        blue1  += blue2;
    }
    
    if( brightness != 255) {
        if( brightness ) {
            brightness++;
            if( red1 )   {
                red1 = scale8( red1, brightness);
                red1++;
            }
            if( green1 ) {
                green1 = scale8( green1, brightness);
                green1++;
            }
            if( blue1 )  {
                blue1 = scale8( blue1, brightness);
                blue1++;
            }
        } else {
            red1 = 0;
            green1 = 0;
            blue1 = 0;
        }
    }
    
    tRGB rgb = {.R = red1, .G = green1, .B = blue1};

    return rgb;
}


void hsv2rgb(const tHSV * hsv, tRGB * rgb)
{
    uint8_t value = hsv->V;
    uint8_t saturation = hsv->S;
    uint8_t hue = ((int)hsv->H * 191) >> 8;

    // The brightness floor is minimum number that all of
    // R, G, and B will be set to.
    uint8_t invsat = 255 - saturation;
    uint8_t brightness_floor = (value * invsat) / 256;

    // The color amplitude is the maximum amount of R, G, and B
    // that will be added on top of the brightness_floor to
    // create the specific hue desired.
    uint8_t color_amplitude = value - brightness_floor;

    // Figure out which section of the hue wheel we're in,
    // and how far offset we are withing that section
    uint8_t section = hue / 0x40; // 0..2
    uint8_t offset = hue % 0x40;  // 0..63

    uint8_t rampup = offset; // 0..63
    uint8_t rampdown = (0x40 - 1) - offset; // 63..0

    // compute color-amplitude-scaled-down versions of rampup and rampdown
    uint8_t rampup_amp_adj   = (rampup   * color_amplitude) / (256 / 4);
    uint8_t rampdown_amp_adj = (rampdown * color_amplitude) / (256 / 4);

    // add brightness_floor offset to everything
    uint8_t rampup_adj_with_floor   = rampup_amp_adj   + brightness_floor;
    uint8_t rampdown_adj_with_floor = rampdown_amp_adj + brightness_floor;

    if( section ) {
        if( section == 1) {
            // section 1: 0x40..0x7F
            rgb->R = brightness_floor;
            rgb->G = rampdown_adj_with_floor;
            rgb->B = rampup_adj_with_floor;
        } else {
            // section 2; 0x80..0xBF
            rgb->R = rampup_adj_with_floor;
            rgb->G = brightness_floor;
            rgb->B = rampdown_adj_with_floor;
        }
    } else {
        // section 0: 0x00..0x3F
        rgb->R = rampdown_adj_with_floor;
        rgb->G = rampup_adj_with_floor;
        rgb->B = brightness_floor;
    }
}



#define K255 255
#define K171 171
#define K170 170
#define K85  85

void hsv2rgb_rainbow(const tHSV * hsv, tRGB * rgb)
{
    // Yellow has a higher inherent brightness than
    // any other color; 'pure' yellow is perceived to
    // be 93% as bright as white.  In order to make
    // yellow appear the correct relative brightness,
    // it has to be rendered brighter than all other
    // colors.
    // Level Y1 is a moderate boost, the default.
    // Level Y2 is a strong boost.
    const uint8_t Y1 = 1;
    const uint8_t Y2 = 0;
    
    // G2: Whether to divide all greens by two.
    // Depends GREATLY on your particular LEDs
    const uint8_t G2 = 0;
    
    // Gscale: what to scale green down by.
    // Depends GREATLY on your particular LEDs
    const uint8_t Gscale = 0;
    
    uint8_t hue = hsv->H;
    uint8_t sat = hsv->S;
    uint8_t val = hsv->V;
    
    uint8_t offset = hue & 0x1F; // 0..31
    
    // offset8 = offset * 8
    uint8_t offset8 = offset;{
        // On ARM and other non-AVR platforms, we just shift 3.
        offset8 <<= 3;
    }
    
    uint8_t third = scale8( offset8, (256 / 3)); // max = 85
    
    volatile uint8_t r, g, b;
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = K255 - third;
                g = third;
                b = 0;
            } else {
                // 001
                //case 1: // O -> Y
                if( Y1 ) {
                    r = K171;
                    g = K85 + third ;
                    b = 0;
                }
                if( Y2 ) {
                    r = K170 + third;
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    g = K85 + twothirds;
                    b = 0;
                }
            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                if( Y1 ) {
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    r = K171 - twothirds;
                    g = K170 + third;
                    b = 0;
                }
                if( Y2 ) {
                    r = K255 - offset8;
                    g = K255;
                    b = 0;
                }
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                g = K255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                g = K171 - twothirds; //K170?
                b = K85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;
                b = K255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = K85 + third;
                g = 0;
                b = K171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = K170 + third;
                g = 0;
                b = K85 - third;
                
            }
        }
    }
    
    // This is one of the good places to scale the green down,
    // although the client can scale green down as well.
    if( G2 ) g = g >> 1;
    if( Gscale ) g = scale8_video( g, Gscale);
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            if( r ) r = scale8_video( r, sat);
            if( g ) g = scale8_video( g, sat);
            if( b ) b = scale8_video( b, sat);

            uint8_t desat = 255 - sat;
            desat = scale8( desat, desat);
            
            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    // Now scale everything down if we're at value < 255.
    if( val != 255 ) {
        val = scale8_video( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            if( r ) r = scale8_video( r, val);
            if( g ) g = scale8_video( g, val);
            if( b ) b = scale8_video( b, val);
        }
    }
    
    // Here we have the old AVR "missing std X+n" problem again
    // It turns out that fixing it winds up costing more than
    // not fixing it.
    // To paraphrase Dr Bronner, profile! profile! profile!
    //asm volatile(  ""  :  :  : "r26", "r27" );
    //asm volatile (" movw r30, r26 \n" : : : "r30", "r31");
    rgb->R = r;
    rgb->G = g;
    rgb->B = b;
}
