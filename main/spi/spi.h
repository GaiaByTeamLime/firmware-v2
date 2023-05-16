#ifndef __SPI_HEADER__
#define __SPI_HEADER__

#include "../prelude.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

#define SPI2_MOSI_PIN 7
#define SPI2_MISO_PIN 2
#define SPI2_SCLK_PIN 6
#define SPI2_QUADWP_PIN -1
#define SPI2_QUADHD_PIN -1
#define SPI2_MAX_TRANSFER_SIZE SOC_SPI_MAXIMUM_BUFFER_SIZE

/**
 * Initialize the SPI2 Host
 */
esp_err_t spi2_init();

/**
 * Send a byte over SPI
 *
 * @param handle The SPI Device handle to send the byte to
 * @param data The byte you want to the to the device
 */
esp_err_t spi_send_byte(spi_device_handle_t* handle, const uint8_t data);

/**
 * Send a byte array over SPI
 *
 * @param handle The SPI Device handle to send the bytes to
 * @param data The byte array containing the data you want to send to the device
 * @param length The length of the byte array
 */
esp_err_t spi_send_bytes(spi_device_handle_t* handle, const uint8_t* data, const uint16_t length);

/**
 * Read a byte from SPI
 * 
 * @param handle The SPI Device handle to send the bytes to
 * @param data The byte you want to the to the device
 */
esp_err_t spi_read_byte(spi_device_handle_t* handle, const uint8_t data);

esp_err_t spi_send_word(spi_device_handle_t* handle, const uint8_t low, const uint8_t high);

#endif
