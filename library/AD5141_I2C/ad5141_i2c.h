#ifndef AD5141_I2C_H_
#define AD5141_I2C_H_


#include <stdio.h>
#include "hal/i2c.h"


// ========================================================== DEFINITIONS ==========================================================
/*! \brief Struct handler for configuring the Digital Potentiometer AD5141 from Analog Devices with I2C interface
* \param i2c_handler    Predefined I2C handler for RP2040
* \param mode_adr       I2C device adresse (will be defined in init function)
* \param init_done      Boolean if device configuration is done        
*/
typedef struct{
    i2c_pico_t *i2c_handler;
    uint8_t adr;
    bool init_done;
} ad5141_i2c_t;

static ad5141_i2c_t DEVICE_AD5141_DEFAULT = {
    .i2c_handler = &DEVICE_I2C_DEFAULT,
    .adr = 0x00,
    .init_done = false
};


// ========================================================== FUNCTIONS ==========================================================
/*! \brief Function for device initialization of Digital Potentiometer AD5141
*   \param device_config    Predefined I2C device handler
*   \param mode_adr         Mode for configuring the I2C bus adresse of device (ADDR0, ADDR1) [0= 0x20 (V_L, V_L), 1= 0x22 (NC, V_L), 2= 0x23 (GND, V_L), 3= 0x28 (V_L, NC), 4= 0x2A (NC, NC), 5= 0x2B (GND, NC), 6= 0x2C V_L, GND), 7= 0x2E (NC, GND), 8= 0x2F (GND, GND)]
*   \return                 bool with init_done
*/
bool ad5141_i2c_init(ad5141_i2c_t *device_config, uint8_t mode_adr);


/*! \brief Function for Software Reset of Digital Potentiometer AD5141
*   \param device_config    Predefined I2C device handler
*/
bool ad5141_i2c_reset_software(ad5141_i2c_t *device_config);


/*! \brief Function for Controling the Shutdown Register of Digital Potentiometer AD5141
*   \param device_config    Predefined I2C device handler
*   \param enable_rdac0     Boolean for enabling the RDAC0
*   \param enable_rdac1     Boolean for enabling the RDAC1
*/
bool ad5141_i2c_control_shutdown(ad5141_i2c_t *device_config, bool enable_rdac0, bool enable_rdac1);


/*! \brief Function for Defining the Wiper Position of selected Channel from Digital Potentiometer AD5141
*   \param device_config    Predefined I2C device handler
*   \param rdac_sel         Number for selecting the wiper (RDAC0, RDAC1)
*   \param pot_position     Potentiometer Position (8-bit)
*/
bool ad5141_i2c_define_level(ad5141_i2c_t *device_config, uint8_t rdac_sel, uint8_t pot_position);

#endif
