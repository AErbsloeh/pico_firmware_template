#include "sens/veml7700.h"


bool VEML7700_init(veml7700_handler_t *handler){
    if(!handler->i2c_mod->init_done){
        configure_i2c_module(handler->i2c_mod);
    }

    // --- Do init of sensor       
    // Load write command and current user register content to buffer to be written (CMD == 0x00)
    uint8_t buffer[3] = {0x00};
    buffer[1] = ((handler->gain & 0x07) << 3) | ((handler->int_time & 0x0C) >> 2);
    buffer[2] = ((handler->int_time & 0x03) << 6);
    
    // Set user register data to be written
    if (handler->en_device) {
        buffer[2] &= ~0x01;
    } else {
        buffer[2] |= 0x01; 
    }

    if (handler->use_isr_thres) {
        buffer[2] |= 0x02;
    } else {
        buffer[2] &= ~0x02; 
    }

    // Send user resgister data to sensor
    if (i2c_write_blocking(handler->i2c_mod->i2c_mod, VEML7700_ADR, buffer, 3, false) == PICO_ERROR_GENERIC) {
        handler->init_done = false;        
    }else{
        handler->init_done = true;
    }
    return handler->init_done;
}


uint16_t VEML7700_read_data(veml7700_handler_t *handler, uint8_t command){
    uint8_t buffer_tx[1] = {command};
    uint8_t buffer_rx[2] = {command};

    if(handler->init_done){
        i2c_write_blocking(handler->i2c_mod->i2c_mod, VEML7700_ADR, buffer_tx, 1, true);
        sleep_us(10);
        while (i2c_read_blocking(handler->i2c_mod->i2c_mod, VEML7700_ADR, buffer_rx, 2, false) == PICO_ERROR_GENERIC){
            sleep_ms(2);
        }; 

        uint16_t raw_data = ((buffer_rx[1] << 8) + buffer_rx[0]); 
        return raw_data;
    } else {
        VEML7700_init(handler);
    }
}


bool VEML7700_read_id(veml7700_handler_t *handler, bool print_id){
    uint16_t id = VEML7700_read_data(handler, 0x07);
    if(print_id){
        printf("Read ID of VEML: %x\n", id);
    };
    return (id == 0xC481) || (id == 0xD481);
}


uint16_t VEML7700_get_als_value(veml7700_handler_t *handler){
    uint16_t raw_als = VEML7700_read_data(handler, 0x04); 
    return raw_als;
}


uint16_t VEML7700_get_white(veml7700_handler_t *handler){
    return VEML7700_read_data(handler, 0x05); 
}
