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
        handler->avai_devices = check_i2c_bus_for_device(handler);
    } 
     
    printf("\n=== Scanning I2C Bus for devices ===\n");
    printf("\".\" = No, \"@\" = Yes\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    int ret;
    uint8_t rxdata;
    for (uint8_t addr=0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(handler->i2c_mod, addr, &rxdata, 1, false);
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    };
    printf("Done.\nAre devices available? -> %x\n", handler->avai_devices);
}


bool check_i2c_bus_for_device(i2c_device_handler_t *handler){
    if(!handler->init_done){
        configure_i2c_module(handler);
        handler->avai_devices = check_i2c_bus_for_device(handler);
    } 

    bool results = false;
    int ret = 0;
    
    uint8_t rxdata;
    for (uint8_t addr=0; addr < (1 << 7); ++addr) {
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(handler->i2c_mod, addr, &rxdata, 1, false);
        if(ret >= 0 && !results)
            results = true;
    };
}


bool construct_i2c_write_data(i2c_device_handler_t *i2c_handler, uint8_t adr, uint8_t buffer_tx[], size_t len_tx){
    uint8_t feedback = i2c_write_blocking(i2c_handler->i2c_mod, adr, buffer_tx, len_tx, false);
    sleep_us(10);
    return feedback != PICO_ERROR_GENERIC;
}


bool construct_i2c_read_data(i2c_device_handler_t *i2c_handler, uint8_t adr, uint8_t buffer_tx[], size_t len_tx, uint8_t buffer_rx[], size_t len_rx){
    i2c_write_blocking(i2c_handler->i2c_mod, adr, buffer_tx, len_tx, true);
    sleep_us(10);
    uint8_t feedback = i2c_read_blocking(i2c_handler->i2c_mod, adr, buffer_rx, len_rx, false);
    sleep_us(10);
    return feedback != PICO_ERROR_GENERIC;
}


uint64_t translate_array_into_uint64(uint8_t buffer_rx[], size_t len_rx){
    uint64_t raw_data = 0;
    for(uint8_t idx = 0; idx < len_rx; idx++){
        raw_data |= buffer_rx[idx] << 8*idx;
    }
    return raw_data;
}
