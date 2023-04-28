#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>

#ifdef DEBUG
#define LOG(format, ...) \
	ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, __func__, format, __VA_ARGS__)
#else
#define LOG(format, ...) /* */
#endif


void app_main(void) {
	int index = 0;
	while (1) {
		LOG("Test! %d", index++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
