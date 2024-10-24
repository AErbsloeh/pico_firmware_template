#include "lib/spi_handler.h"


bool configure_spi_module(spi_device_handler_t *handler, bool use_spi_slave){
    // --- Init of GPIOs
    // GPIO: CS
    gpio_init(handler->pin_cs);
    gpio_set_dir(handler->pin_cs, GPIO_OUT);
    gpio_set_drive_strength(handler->pin_cs, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(handler->pin_cs, true);

    // GPIO: MOSI, SCLK, MISO
    gpio_set_function(handler->pin_cs, GPIO_FUNC_SPI);
    gpio_set_function(handler->pin_sclk, GPIO_FUNC_SPI);
    gpio_set_function(handler->pin_mosi, GPIO_FUNC_SPI);
    gpio_set_function(handler->pin_miso, GPIO_FUNC_SPI);

    // --- Init of SPI module
    spi_init(handler->spi_mod, handler->fspi_khz * 1000);
    spi_set_slave(handler->spi_mod, use_spi_slave);
    spi_set_format(
        handler->spi_mod,   
        handler->bits_per_transfer, 
        (handler->mode >= 2) ? SPI_CPOL_1 : SPI_CPOL_0,
        (handler->mode % 2 == 1) ? SPI_CPHA_1 : SPI_CPHA_0,
        (handler->msb_first) ? SPI_MSB_FIRST : SPI_LSB_FIRST
    );

    handler->init_done = true;
    return handler->init_done;
}


bool configure_spi_module_soft(spi_device_handler_t *handler){
    // GPIO: CS
    gpio_init(handler->pin_cs);
    gpio_set_dir(handler->pin_cs, GPIO_OUT);
    gpio_set_drive_strength(handler->pin_cs, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(handler->pin_cs, true);

    // GPIO: SCLK
    gpio_init(handler->pin_sclk);
    gpio_set_dir(handler->pin_sclk, GPIO_OUT);
    gpio_set_drive_strength(handler->pin_sclk, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(handler->pin_sclk, (handler->mode == 2) || (handler->mode == 3));

    // GPIO: MOSI
    gpio_init(handler->pin_mosi);
    gpio_set_dir(handler->pin_mosi, GPIO_OUT);
    gpio_set_drive_strength(handler->pin_mosi, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(handler->pin_mosi, false);

    // GPIO: MISO
    gpio_init(handler->pin_miso);
    gpio_set_dir(handler->pin_miso, GPIO_IN);
    gpio_set_input_hysteresis_enabled(handler->pin_miso, true);
    gpio_set_drive_strength(handler->pin_miso, GPIO_DRIVE_STRENGTH_2MA);

    handler->init_done = true;
    return handler->init_done;
}


uint16_t send_data_spi_module_soft(spi_device_handler_t *handler, uint16_t data){
    uint16_t data_returned = 0;
    uint8_t position_send = (handler->msb_first) ? (uint8_t)handler->bits_per_transfer-1 : 0;
    bool cpol = (handler->mode == 2) || (handler->mode == 3);
    bool cpha = (handler->mode % 2 == 1);
    
    gpio_put(handler->pin_cs, false);
    for(uint8_t cnt=0; cnt < handler->bits_per_transfer; cnt++){
        // Starting condition
        if(cpha && cnt == 0) {
            sleep_us(1);
        } else {
            gpio_put(handler->pin_mosi, (data >> position_send) & 0x0001);
        };

        // First edge
        gpio_put(handler->pin_sclk, !cpol);
        if(cpha){
            gpio_put(handler->pin_mosi, (data >> position_send) & 0x0001);
        } else {
            data_returned |= (gpio_get(handler->pin_miso) << position_send);
        };
        sleep_us(1);

        // Second edge
        gpio_put(handler->pin_sclk, cpol);
        if(cpha){
            data_returned |= (gpio_get(handler->pin_miso) << position_send);
        } else {
            gpio_put(handler->pin_mosi, cnt % 2 == 1);
        };
        sleep_us(1);
        position_send = (handler->msb_first) ? position_send - 1 : position_send + 1;
    };
    gpio_put(handler->pin_cs, true);
    gpio_put(handler->pin_sclk, cpol);

    return data_returned;
}
