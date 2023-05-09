
#include "prelude.h"

#include "wifi/wifi.h"

esp_err_t on_wifi_connect(void) {
	LOG("Yay we connected!");
	return ESP_OK;
}

void app_main(void) {
	LOG("Init");

	wifi_init(&on_wifi_connect);
	wifi_start("TestSpot", "baguette");
}
