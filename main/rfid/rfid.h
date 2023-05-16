#ifndef __RFID_HEADER__
#define __RFID_HEADER__

#include "../prelude.h"

#include "rfid_pcd_register_types.h"

#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

#define RFID_SPI_HOST SPI2_HOST
#define RFID_SPI_MODE 0
#define RFID_SPI_CS_PIN 10
#define RFID_SPI_CLK_SPD (4 * 1000 * 1000)
#define RFID_SPI_QUEUE_SIZE 1

esp_err_t rfid_init(spi_device_handle_t* handle);

/**
 * Send data to a register to the RC522
 *
 * @param handle The SPI device handle
 * @param reg The register to send data too
 * @param data The data to set to the register
 */
esp_err_t rfid_send_register(
	spi_device_handle_t* handle, rfid_pcd_register_t reg, uint8_t data
);
/**
 * Send a command to the RC522
 *
 * This is basically just an alias to rfid_send_register(COMMAND_REG, command)
 *
 * @param handle The SPI device handle
 * @param data The data to set to the register
 */
esp_err_t
rfid_send_command(spi_device_handle_t* handle, rfid_pcd_command_t command);

esp_err_t rfid_read_registers(
	spi_device_handle_t* handle, const rfid_pcd_register_t* registers,
	uint8_t* buffer, const uint16_t length
);

#endif
