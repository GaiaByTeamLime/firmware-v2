#include "spi.h"

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
