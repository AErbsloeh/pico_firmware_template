#include "lib/i2c_handler.h"


bool configure_i2c_module(i2c_device_handler_t *handler){
    i2c_init(handler->i2c_mod, handler->fi2c_khz * 1000);

    gpio_set_function(handler->pin_sda, GPIO_FUNC_I2C);
    gpio_set_function(handler->pin_scl, GPIO_FUNC_I2C);
    gpio_pull_up(handler->pin_sda);
    gpio_pull_up(handler->pin_scl);

    handler->init_done = true;
    return handler->init_done;
}


bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}


void scan_i2c_bus_for_device(i2c_device_handler_t *handler){
    if(!handler->init_done){
        configure_i2c_module(handler);
    } 
     
    printf("\n=== Scanning I2C Bus for devices ===\n");
    printf("\".\" = No, \"@\" = Yes\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (uint8_t addr=0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.
        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(handler->i2c_mod, addr, &rxdata, 1, false);
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}