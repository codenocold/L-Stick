
// ColorPalette


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    tRGB rgb;
    uint8_t brightness = 255;
    
    for( int i = 0; i < WS2812B_NUMLEDS; i++) {
        rgb = ColorFromPalette(CloudColors_p, colorIndex, brightness, LINEARBLEND);
        WS2812B_setColorRGB(&rgb, i, false);
        colorIndex += 3;
    }

    WS2812B_update();
}

int main(void)
{
    NRF_LOG_INIT(NULL);
    NRF_LOG_DEFAULT_BACKENDS_INIT();

    WS2812B_init();

    static uint8_t startIndex = 0;

    while (true){
        startIndex = startIndex + 1; /* motion speed */
        
        FillLEDsFromPaletteColors( startIndex);
        
        nrf_delay_ms(10);
    }
}
