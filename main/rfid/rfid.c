#include "rfid.h"
#include "driver/spi_master.h"

esp_err_t rfid_init(spi_device_handle_t* handle) {
	const spi_device_interface_config_t device_config = {
		.mode = SPI_TRANS_MODE_DIO,
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
