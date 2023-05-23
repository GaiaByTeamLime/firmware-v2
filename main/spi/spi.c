#include "spi.h"
#include "driver/spi_master.h"
#include "prelude.h"

esp_err_t spi2_init() {
	const spi_bus_config_t bus_config = {
		.mosi_io_num = SPI2_MOSI_PIN,
		.miso_io_num = SPI2_MISO_PIN,
		.sclk_io_num = SPI2_SCLK_PIN,
		.quadwp_io_num = SPI2_QUADWP_PIN,
		.quadhd_io_num = SPI2_QUADHD_PIN,
		.max_transfer_sz = SPI2_MAX_TRANSFER_SIZE,
	};

	PASS_ERROR(
		spi_bus_initialize(SPI2_HOST, &bus_config, SPI_DMA_CH_AUTO),
		"Could not initialize SPI2 Host"
	);
	LOG("SPI2 host initialize");

	return ESP_OK;
}

esp_err_t spi_send_byte(spi_device_handle_t* handle, const uint8_t data) {
	uint8_t tx_buffer[1] = {data};

	spi_transaction_t transaction = {0};
	transaction.tx_buffer = tx_buffer;
	transaction.length = 8;

	return spi_device_transmit(*handle, &transaction);
}

esp_err_t spi_send_datastream(
	spi_device_handle_t* handle, const uint8_t* data, const uint16_t length
) {
	spi_transaction_t transaction = {0};
	transaction.tx_buffer = data;
	transaction.length = length * 8;

	return spi_device_transmit(*handle, &transaction);
}

esp_err_t spi_send_word(spi_device_handle_t* handle, const uint16_t word) {
	uint8_t tx_buffer[2] = {0};

	tx_buffer[0] = word >> 8;
	tx_buffer[1] = word;

	spi_transaction_t transaction = {0};
	transaction.tx_buffer = tx_buffer;
	transaction.length = 16;

	return spi_device_transmit(*handle, &transaction);
}
