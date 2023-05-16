#include <stdlib.h>

#include "prelude.h"
#include "rfid.h"
#include "spi.h"

esp_err_t rfid_init(spi_device_handle_t* handle) {
	const spi_device_interface_config_t device_config = {
		.mode = RFID_SPI_MODE,
		.spics_io_num = RFID_SPI_CS_PIN,
		.clock_speed_hz = RFID_SPI_CLK_SPD,
		.queue_size = RFID_SPI_QUEUE_SIZE,
	};

	PASS_ERROR(
		spi_bus_add_device(RFID_SPI_HOST, &device_config, handle),
		"Unable to add RFID SPI device to SPI host"
	);

	return ESP_OK;
}

esp_err_t rfid_send_register(
	spi_device_handle_t* handle, rfid_pcd_register_t reg, uint8_t data
) {
	uint8_t send_data[2] = {reg, data};
	return spi_send_bytes(handle, send_data, 2);
}

esp_err_t
rfid_send_command(spi_device_handle_t* handle, rfid_pcd_command_t command) {
	return rfid_send_register(handle, COMMAND_REG, command);
}

esp_err_t rfid_read_registers(
	spi_device_handle_t* handle, const rfid_pcd_register_t* registers,
	uint8_t* buffer, const uint16_t length
) {
	rfid_pcd_register_t read_registers[length];

	// Make sure the registers we send are marked as 'read'
	for (uint16_t index = 0; index < length; index++) {
		read_registers[index] = registers[index] | 0x80;
	}

	// Create small buffers for each transaction
	uint8_t current_register[2] = {0}; // [0] = The register, [1] = Always 0
	uint8_t rx_buffer[2]; // [0] = Ignore, [1] = The data
	for (uint16_t index = 0; index < length; index++) {
		current_register[0] = read_registers[index];

		spi_transaction_t transaction = {0};
		transaction.tx_buffer = current_register;
		transaction.length = 16;
		transaction.rx_buffer = rx_buffer;

		PASS_ERROR(spi_device_transmit(*handle, &transaction), "Unable to transmit");

		buffer[index] = rx_buffer[1];
	}

	return ESP_OK;
}
