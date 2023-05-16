#include <stdlib.h>

#include "spi.h"
#include "rfid.h"

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

esp_err_t rfid_send_register(spi_device_handle_t* handle, rfid_pcd_register_t reg, uint8_t data) {
	uint8_t send_data[2] = { reg, data };
	return spi_send_bytes(handle, send_data, 2);
}

esp_err_t rfid_send_command(spi_device_handle_t* handle, rfid_pcd_command_t command) {
	return rfid_send_register(handle, COMMAND_REG, command);
}

esp_err_t rfid_read_registers(
	spi_device_handle_t* handle, const rfid_pcd_register_t* registers, uint8_t* buffer, const uint16_t length
) {
	uint8_t shifted_buffer[length];

	spi_transaction_t transaction = {0};
	transaction.tx_buffer = registers;
	transaction.length = (length * 2) * 8;
	transaction.rx_buffer = shifted_buffer;

	// Transmit
	PASS_ERROR(spi_device_transmit(*handle, &transaction), "Unable to transmit");

	// Copy the shifted buffer into the correct position
	for (uint16_t index = 1; index < length + 1; index++) {
		buffer[index - 1] = shifted_buffer[index];
	}

	return ESP_OK;
}
