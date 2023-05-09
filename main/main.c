#include "prelude.h"

#include "adc/adc.h"
#include "persistent_storage/persistent_storage.h"

#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

void app_main(void) {
	int index = 0;

	esp_err_t message = adc_init();
	if (message != ESP_OK) {
		while (1) {
			LOG("ADC1 Init Error");
		}
	}

	persistent_storage_init();

	char ssid[WIFI_SSID_CHAR_BUFFER_LENGTH];
	char passwd[WIFI_PASSWORD_CHAR_BUFFER_LENGTH];
	persistent_storage_get_wifi(ssid, passwd);

	LOG("SSID: %s\tPassword: %s", ssid, passwd);

	while (1) {
		pull_latest_data();
		uint32_t adc_data;
		message = get_adc_data(ADC1_LDR, &adc_data);
		if (message == ESP_OK) {
			LOG("LDR Data: %lu", adc_data);
		}
		LOG("Test! %d", index++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
