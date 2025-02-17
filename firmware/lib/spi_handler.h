#ifndef SPI_HANDLER_H
#define SPI_HANDLER_H


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"


/*! \brief Struct handler for configuring the SPI interface of RP2040
* \param pin_mosi           GPIO num of used MOSI
* \param pin_sclk           GPIO num of used SCLK
* \param pin_miso           GPIO num of used MISO
* \param spi_mod            SPI handler of RP2040 (spi0 or spi1)
* \param fspi_khz           Used SPI clock [in kHz]
* \param mode               Used SPI mode
* \param msb_first          Send MSB first in transmission
* \param bits_per_transfer  Bit length in each SPI transmission
* \param init_done          Boolean if initilization of SPI module is done
*/
typedef struct{
    uint8_t pin_mosi;
    uint8_t pin_sclk;
    uint8_t pin_miso;
    spi_inst_t *spi_mod;
    uint16_t fspi_khz;
    uint8_t mode;
    bool msb_first;
    uint8_t bits_per_transfer;
    bool init_done;
} spi_device_handler_t;

static spi_device_handler_t DEVICE_SPI_DEFAULT = {
    .pin_mosi = PICO_DEFAULT_SPI_TX_PIN,
    .pin_sclk = PICO_DEFAULT_SPI_SCK_PIN,
    .pin_miso = PICO_DEFAULT_SPI_RX_PIN,
    .spi_mod = PICO_DEFAULT_SPI,
    .fspi_khz = 1000,
    .mode = 0,
    .msb_first = true,
    .bits_per_transfer = 16,
    .init_done = false
};


/*! \brief Function for configuring the SPI interface of RP2040
* \param handler        Pointer to struct for setting-up the SPI interface module
* \param gpio_num_csn   GPIO number of used CSN (default: PICO_DEFAULT_SPI_CSN_PIN)
* \param use_spi_slave  Boolean if module is used as slave
* \return   Bool if configuration of SPI module was successful
*/
bool configure_spi_module(spi_device_handler_t *handler, uint8_t gpio_num_csn, bool use_spi_slave);


/*! \brief Function for sending data via hardware-defined SPI interface of the RP2040
* \param handler        Pointer to struct for setting-up the SPI interface module
* \param gpio_num_csn   GPIO number of used CSN (default: PICO_DEFAULT_SPI_CSN_PIN)
* \param data_tx        Data array (uint8_t) for sending data
* \param length         Number of bytes to send/receive
* \return               Number of written/read bytes
*/
int8_t send_data_spi_module(spi_device_handler_t *handler, uint8_t gpio_num_csn, uint8_t data_tx[], size_t length);


/*! \brief Function for sending and receiving data via hardware-defined SPI interface of the RP2040
* \param handler        Pointer to struct for setting-up the SPI interface module
* \param gpio_num_csn   GPIO number of used CSN (default: PICO_DEFAULT_SPI_CSN_PIN)
* \param data_tx        Data array (uint8_t) for sending data
* \param data_rx        Data array (uint8_t) for getting data
* \param length         Number of bytes to send/receive
* \return               Number of written/read bytes
*/
int8_t receive_data_spi_module(spi_device_handler_t *handler, uint8_t gpio_num_csn, uint8_t data_tx[], uint8_t data_rx[], size_t length);


/*! \brief Function for configuring a software-defined SPI interface for RP2040
* \param handler        Pointer to struct for setting-up the SPI interface module
* \param gpio_num_csn   GPIO number of used CSN (default: PICO_DEFAULT_SPI_CSN_PIN)
* \return   Bool if configuration of SPI module was successful
*/
bool configure_spi_module_soft(spi_device_handler_t *handler, uint8_t gpio_num_csn);


/*! \brief Function for sending data via software-defined SPI interface
* \param handler        Pointer to struct for setting-up the SPI interface module
* \param gpio_num_csn   GPIO number of used CSN (default: PICO_DEFAULT_SPI_CSN_PIN)
* \param data           Data array (uint8_t) for sending and getting data
* \return   Bool if configuration of SPI module was successful
*/
uint16_t send_data_spi_module_soft(spi_device_handler_t *handler, uint8_t gpio_num_csn, uint16_t data);


/*! \brief Function with RPi Pico constructor for processing data from buffer 
* \param buffer_tx      uint8 arrray with data to receive from SPI bus
* \param size           Length of array
* \return               Value
*/
uint32_t translate_array_into_uint32(uint8_t buffer_rx[], size_t len_rx);


#endif
