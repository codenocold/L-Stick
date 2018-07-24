#include "storage.h"
#include "nrf_fstorage.h"

#ifdef SOFTDEVICE_PRESENT
#include "nrf_fstorage_sd.h"
#else
#include "nrf_fstorage_nvmc.h"
#endif

#include "nrf_log.h"


//----------------------------------------------------------------------------------
// Varable Definitions
int32_t G_ModeIndex;
tModeTable *G_pModeTable;
tStaticLight *G_pStaticLight;
tIMG *G_pIMG0;
tIMG *G_pIMG1;
tIMG *G_pIMG2;
tIMG *G_pIMG3;
tIMG *G_pIMG4;
tIMG *G_pIMG5;


//----------------------------------------------------------------------------------
// Function Declarations
static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
static int32_t STORAGE_get_mode_index(void);
static void STORAGE_save_mode_index(void);


NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    .evt_handler = fstorage_evt_handler,
    .start_addr = STORAGE_ADDR_START,
    .end_addr   = STORAGE_ADDR_END,
};


void STORAGE_init(void)
{
    ret_code_t ret;

    nrf_fstorage_api_t * p_fs_api;

#ifdef SOFTDEVICE_PRESENT
    p_fs_api = &nrf_fstorage_sd;
#else
    p_fs_api = &nrf_fstorage_nvmc;
#endif
    ret = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(ret);

    G_ModeIndex = STORAGE_get_mode_index();
    G_pModeTable = (tModeTable*)MODE_TABLE_ADDR;
    G_pStaticLight = (tStaticLight*)STATIC_LIGHT_ADDR;
    G_pIMG0 = (tIMG*)IMG0_ADDR;
    G_pIMG1 = (tIMG*)IMG1_ADDR;
    G_pIMG2 = (tIMG*)IMG2_ADDR;
    G_pIMG3 = (tIMG*)IMG3_ADDR;
    G_pIMG4 = (tIMG*)IMG4_ADDR;
    G_pIMG5 = (tIMG*)IMG5_ADDR;
}

void STORAGE_deinit(void)
{
    STORAGE_save_mode_index();
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

static int32_t STORAGE_get_mode_index(void)
{
    int32_t index;

    for(uint32_t addr=MODE_INDEX_ADDR_START; addr<MODE_INDEX_ADDR_END; addr+=4){
        index = *(int32_t*)addr;
        if(index == (int32_t)0xFFFFFFFF || index != 0){
            break;
        }
    }

    if(index < 1 || index > 16){
        index = 1;
    }

    return index-1;
}

static void STORAGE_save_mode_index(void)
{
    uint32_t ret;
    uint32_t addr;
    int32_t NewIndex;

    if(G_ModeIndex == STORAGE_get_mode_index()){
        return;
    }

    for(addr=MODE_INDEX_ADDR_START; addr<MODE_INDEX_ADDR_END; addr+=4){
        if(*(int32_t*)addr == (int32_t)0xFFFFFFFF){
            // Write new mode index
            NewIndex = G_ModeIndex+1;
            ret = nrf_fstorage_write(&fstorage, addr, &NewIndex, sizeof(NewIndex), NULL);
            APP_ERROR_CHECK(ret);
            while (nrf_fstorage_is_busy(&fstorage));

            if(addr > MODE_INDEX_ADDR_START){
                // Clear old mode index
                NewIndex = 0;
                addr -= 4;
                ret = nrf_fstorage_write(&fstorage, addr, &NewIndex, sizeof(NewIndex), NULL);
                APP_ERROR_CHECK(ret);
                while (nrf_fstorage_is_busy(&fstorage));
            }

            break;
        }
    }

    if(addr >= MODE_INDEX_ADDR_END){
        // Erase mode index page
        ret = nrf_fstorage_erase(&fstorage, MODE_INDEX_ADDR_START, 1, NULL);
        APP_ERROR_CHECK(ret);
        while (nrf_fstorage_is_busy(&fstorage));

        // Write new mode index
        NewIndex = G_ModeIndex-1;
        ret = nrf_fstorage_write(&fstorage, MODE_INDEX_ADDR_START, &NewIndex, sizeof(NewIndex), NULL);
        APP_ERROR_CHECK(ret);
        while (nrf_fstorage_is_busy(&fstorage));
    }
}

void STORAGE_update_mode_table(tModeTable *pModeTable)
{
    uint32_t ret;

    // Erase mode table page
    ret = nrf_fstorage_erase(&fstorage, MODE_TABLE_ADDR, 1, NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));

    // Write new mode tabel
    ret = nrf_fstorage_write(&fstorage, MODE_TABLE_ADDR, pModeTable, sizeof(tModeTable), NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));
}

void STORAGE_update_static_light(tStaticLight *pStaticLight)
{
    uint32_t ret;

    // Erase static light page
    ret = nrf_fstorage_erase(&fstorage, STATIC_LIGHT_ADDR, 1, NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));

    // Write static light
    ret = nrf_fstorage_write(&fstorage, STATIC_LIGHT_ADDR, pStaticLight, sizeof(tStaticLight), NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));
}

void STORAGE_erase_img(uint8_t img)
{
    uint32_t ret;
    uint32_t addr;

    switch(img){
        case 0:
            addr = IMG0_ADDR;
            break;

        case 1:
            addr = IMG1_ADDR;
            break;  

        case 2:
            addr = IMG2_ADDR;
            break;

        case 3:
            addr = IMG3_ADDR;
            break;

        case 4:
            addr = IMG4_ADDR;
            break;

        case 5:
            addr = IMG5_ADDR;
            break;

        default:
            return;
    }

    // Erase img page
    ret = nrf_fstorage_erase(&fstorage, addr, 1, NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));
}

void STORAGE_prog_img(uint8_t img, uint32_t offset, void const *p_src, uint32_t len)
{
    uint32_t ret;
    uint32_t addr;

    switch(img){
        case 0:
            addr = IMG0_ADDR;
            break;

        case 1:
            addr = IMG1_ADDR;
            break;  

        case 2:
            addr = IMG2_ADDR;
            break;

        case 3:
            addr = IMG3_ADDR;
            break;

        case 4:
            addr = IMG4_ADDR;
            break;

        case 5:
            addr = IMG5_ADDR;
            break;

        default:
            return;
    }

    // Program
    ret = nrf_fstorage_write(&fstorage, addr, p_src, len, NULL);
    APP_ERROR_CHECK(ret);
    while (nrf_fstorage_is_busy(&fstorage));
}