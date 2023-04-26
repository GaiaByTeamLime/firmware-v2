
#include "prelude.h"

void app_main(void) {
	int index = 0;
	while (1) {
		LOG("Test! %d", index++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
