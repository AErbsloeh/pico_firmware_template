#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "wrapper/spi_handler.h"
#include "sens/ads8881.h"


int main(){ 
    static ads8881_handler_t setting_device = {
        .spi_handler = &DEVICE_SPI_DEFAULT,
        .gpio_num_csn = 1,
        .init_done = false
    };

    // Init of device
    ads8881_init(&setting_device);

    //Main Loop for communication
    uint8_t pos[1] = {0};
    while (true){
        
        sleep_ms(250);
    };
}
