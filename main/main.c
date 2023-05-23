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

	// Wake up the rfid reader
	rfid_wakeup_mifare_tag(&rfid_handle);

	// Read actual data from the tag
	uint8_t buffer[16] = {0};
	rfid_read_mifare_tag(&rfid_handle, 4, buffer, 16);

	// Output the data
	LOG("SUCCES:");
	for (uint8_t i = 0; i < 16; i++) {
		LOG("\t%x (%c)", buffer[i], buffer[i]);
	}
	LOG("END");

	// Read actual data from the tag
	rfid_read_mifare_tag(&rfid_handle, 4 + 4, buffer, 16);

	// Output the data
	LOG("SUCCES:");
	for (uint8_t i = 0; i < 16; i++) {
		LOG("\t%x (%c)", buffer[i], buffer[i]);
	}
	LOG("END");


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
