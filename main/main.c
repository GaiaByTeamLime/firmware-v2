#include "prelude.h"

#include "adc/adc.h"
#include "persistent_storage/persistent_storage.h"
#include "rfid/rfid.h"
#include "rfid/rfid_pcd_register_types.h"
#include "spi/spi.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

esp_err_t setup(spi_device_handle_t* rfid_spi_handle) {
	PASS_ERROR(adc_init(), "ADC1 Init Error");
	PASS_ERROR(
		persistent_storage_init(), "Could not initialize persistent storage"
	);

	PASS_ERROR(spi2_init(), "Could not initialize SPI2 Host");
	PASS_ERROR(rfid_init(rfid_spi_handle), "Could not add RFID to SPI Host");

	return ESP_OK;
}

void app_main(void) {
	spi_device_handle_t rfid_handle = {0};
	setup(&rfid_handle);

	LOG("START");
	rfid_pcd_register_t registers[3] = { MODE_REG, TX_CONTROL_REG, TX_SEL_REG };
	uint8_t result[3];
	LOG("PRE-READ");
	rfid_read_registers(&rfid_handle, registers, result, 3);
	LOG("-> 0x%02x 0x%02x 0x%02x", result[0], result[1], result[2]);

	LOG("SENDING BYTES");
	spi_send_word(&rfid_handle, MODE_REG, 0x3e);
	// spi_send_byte(&rfid_handle, 0x3e);

	LOG("READ 2");
	rfid_read_registers(&rfid_handle, registers, result, 3);
	LOG("-> 0x%02x 0x%02x 0x%02x", result[0], result[1], result[2]);

	// while (1) {
	// 	pull_latest_data();
	// 	uint32_t adc_data;
	// 	message = get_adc_data(ADC1_LDR, &adc_data);
	// 	if (message == ESP_OK) {
	// 		LOG("LDR Data: %lu", adc_data);
	// 	}
	// 	LOG("Test! %d", index++);
	// 	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// }
}
