#ifndef _STORAGE_H_
#define _STORAGE_H_


#include "nrf.h"
#include "led.h"


#define STORAGE_ADDR_START			0x00073000
#define MODE_INDEX_ADDR_START		0x00073000	
#define MODE_INDEX_ADDR_END			0x00074000	

#define MODE_TABLE_ADDR				0x00074000	
#define STATIC_LIGHT_ADDR			0x00075000	
#define IMG0_ADDR					0x00076000
#define IMG1_ADDR					0x00077000
#define IMG2_ADDR					0x00078000
#define IMG3_ADDR					0x00079000
#define IMG4_ADDR					0x0007A000
#define IMG5_ADDR					0x0007B000
#define STORAGE_ADDR_END			0x0007C000	/* 0x0007C000~0x0080000 (4 pages)used by peer manager fds*/


typedef struct sModeTable
{
	uint8_t table[16];
}tModeTable;

typedef struct sStaticLight
{
	tRGB color[6];
}tStaticLight;

typedef struct sIMG
{
	tRGB color[48][16];
}tIMG;


extern int32_t G_ModeIndex;
extern tModeTable *G_pModeTable;
extern tStaticLight *G_pStaticLight;
extern tIMG *G_pIMG0;
extern tIMG *G_pIMG1;
extern tIMG *G_pIMG2;
extern tIMG *G_pIMG3;
extern tIMG *G_pIMG4;
extern tIMG *G_pIMG5;


void STORAGE_init(void);
void STORAGE_deinit(void);
void STORAGE_update_mode_table(tModeTable *pModeTable);
void STORAGE_update_static_light(tStaticLight *pStaticLight);
void STORAGE_erase_img(uint8_t img);
void STORAGE_prog_img(uint8_t img, uint32_t offset, void const *p_src, uint32_t len);


#endif
