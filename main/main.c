#include "prelude.h"

#include "adc/adc.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <sdkconfig.h>

void app_main(void) {
	int index = 0;

	esp_err_t message = adc_init();
	if (message != ESP_OK)
	{
		while (1)
		{
			LOG("ADC1 Init Error: %s", esp_err_to_name(message));
		}
	}

	while (1) {
		struct ADC_data data = getData();
		if (data.messageResult != ESP_OK)
		{
			LOG("ADC1 Get Data Error: %s", esp_err_to_name(data.messageResult));
		} else {
		    LOG("LDR Data: %d", data.data[0]);
		}
		LOG("Test! %d", index++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
