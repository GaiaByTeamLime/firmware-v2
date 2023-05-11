#include "prelude.h"

#include "adc/adc.h"
#include "persistent_storage/persistent_storage.h"
#include "spi/spi.h"
#include "rfid/rfid.h"

#include <esp_err.h>
#include <driver/spi_master.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void) {
	// int index = 0;

	persistent_storage_init();
	esp_err_t message = adc_init();
	if (message != ESP_OK) {
		while (1) {
			LOG("ADC1 Init Error");
		}
	}

	spi2_init();

	spi_device_handle_t rfid_handle;
	rfid_init(&rfid_handle);

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
