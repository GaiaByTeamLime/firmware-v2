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
#define RFID_SPI_CLK_SPD (2 * 1000 * 1000)
#define RFID_SPI_QUEUE_SIZE 1

/**
 * Send data to a register to the RC522
 *
 * @param handle The SPI device handle
 * @param reg The register to send data too
 * @param data The data to set to the register
 */
esp_err_t rfid_write_register(
	spi_device_handle_t* handle, rfid_pcd_register_t reg, uint8_t data
);

/**
 * Write an array of data to a single register, the inverse of
 * `rfid_read_register_datastream`. This function is mainly used to write data
 * to the FIFO data register.
 *
 * @param handle The SPI device handle
 * @param reg The register you want to write to
 * @param datastream The data you want to store in the buffer
 * @param length The size of the datastream buffer
 */
esp_err_t rfid_write_register_datastream(
	spi_device_handle_t* handle,
	rfid_pcd_register_t reg,
	uint8_t* datastream,
	const uint16_t length
);

/**
 * Read a datastream from a single register, this differs significantly from the
 * `rfid_read_registers`. This function sends one long request to a single
 * register, attempting to read several bytes of data out of it. This is used to
 * read from the FIFO data register.
 *
 * @param handle The SPI device handle
 * @param reg The register you want to read from
 * @param output_stream The output buffer the data from the register is stored
 * in
 * @param length The size of the output_stream buffer
 */
esp_err_t rfid_read_register_datastream(
	spi_device_handle_t* handle,
	rfid_pcd_register_t reg,
	uint8_t* output_stream,
	const uint16_t length
);

/**
 * Send a command to the RC522
 *
 * This is basically just an alias to rfid_send_register(COMMAND_REG, command)
 *
 * @param handle The SPI device handle
 * @param data The data to set to the register
 */
esp_err_t rfid_send_command(
	spi_device_handle_t* handle, rfid_pcd_command_t command
);

/**
 * Read a register from the RFID reader
 *
 * @param handle The SPI device handle
 * @param registers An array of registers one wants to read from
 * @param buffer The output buffer, the values of the requested registers will
 * be placed here
 * @param length How many registers you want to read, the output buffer and the
 * register array size must be the same
 */
esp_err_t rfid_read_registers(
	spi_device_handle_t* handle,
	const rfid_pcd_register_t* registers,
	uint8_t* buffer,
	const uint16_t length
);

/**
 * Try to communicate with an MiFare tag on the RFID reader. It will write data
 * to the tag and then wait for return data. This function does block until the
 * RFID timer timed out, or we received our return data.
 *
 * @param handle The SPI device handle
 * @param write_data The data to write to tag
 * @param write_length The size of write_data array
 * @param read_data The buffer the output data from the tag gets stored in
 * @param read_length The length of the output data buffer
 * @param framing_bits The framing bits for the transceive, see the MiFare tag
 * datasheet for which MiFare commands need how many framing bits
 */
esp_err_t rfid_transceive(
	spi_device_handle_t* handle,
	uint8_t* write_data,
	const uint16_t write_length,
	uint8_t* read_data,
	const uint16_t read_length,
	const uint8_t framing_bits
);

/**
 * Calculate an CRC checksum over an array of bytes. This checksum calculated on
 * the RFID reader side. For that reason, this is a blocking function.
 *
 * @param handle The SPI device handle
 * @param data_to_calculate_crc The data array to calculate the crc checksum
 * over
 * @param length The length of the data array
 * @param result An two-byte array where the result of the CRC checksum gets
 * calculated over
 */
esp_err_t rfid_calculate_crc(
	spi_device_handle_t* handle,
	uint8_t* data_to_calculate_crc,
	const uint16_t length,
	uint8_t* result
);

/**
 * Read 16 bytes of data from an MiFare tag. In order for this function to work
 * the state of the MiFare tag MUST be in READY. If this is not the case, this
 * function may time out and return invalid data. This function is blocking.
 *
 * @param handle The SPI device handle
 * @param block_address The block address of the tag to read data from. Each
 * block is 4 bytes wide. User-data starts from address 4. For other useful
 * address refer to the datasheet of the MiFare tag.
 * @param buffer The output buffer, this array MUST be AT LEAST 16 bytes of data
 * length. As a read request to an MiFare device returns 16 bytes of data.
 * @param buffer_size The size of the output buffer, as stated before, this must
 * be AT LEAST 16 bytes.
 */
esp_err_t rfid_read_mifare_tag(
	spi_device_handle_t* handle,
	uint8_t block_address,
	uint8_t* buffer,
	uint16_t buffer_size
);

/**
 * Enable the antenna of the RFID reader
 *
 * @param The SPI device handle
 */
esp_err_t rfid_enable_antenna(spi_device_handle_t* handle);

/**
 * Disable the antenna of the RFID reader
 *
 * @param The SPI device handle
 */
esp_err_t rfid_disable_antenna(spi_device_handle_t* handle);

/**
 * Wakes the tag up from IDLE mode into READY mode. The tag must be in READY
 * mode if you want to read data from it. Any read attempts where this is not
 * the case will result into invalid data.
 *
 * @param The SPI device handle
 */
esp_err_t rfid_wakeup_mifare_tag(spi_device_handle_t* handle);

/**
 * Initializes the RFID reader. This function also invokes the
 * `rfid_enable_antenna` function. You do not need to manually call this.
 *
 * If a tag is placed on the reader, the tag will be set to IDLE mode. Thus
 * you need to invoke `rfid_wakeup_mifare_tag` in-order to read any data from
 * an MiFare tag.
 *
 * @param The SPI device handle
 */
esp_err_t rfid_init(spi_device_handle_t* handle);

#endif
