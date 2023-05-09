
#include "../prelude.h"

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#include "esp_err.h"
#include "esp_wifi_types.h"
#include "wifi.h"

esp_err_t (*wifi_connected_callback)(void);

static void ip_event_handler(void *arg, esp_event_base_t event_base,
							 int32_t event_id, void *event_data) {
	// We got a connection!
	if (event_id == IP_EVENT_STA_GOT_IP) {
		wifi_connected_callback();
	}
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data) {
	// Just keep retrying on disconnects
	if (event_id == WIFI_EVENT_STA_DISCONNECTED || event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	}
}

esp_err_t wifi_start(const char *ssid, const char *password) {
	// Create a config struct
	wifi_config_t wifi_config = {.sta = {
									 .ssid = {0}, // Make the SSID and Password blank, we don't want random bits in these fields
									 .password = {0},
									 .threshold.authmode = WIFI_AUTH_WPA2_PSK,
								 }};

	// Copy the SSID and the password to the wifi config
	uint8_t copy_index = 0;
	while (*ssid) {
		wifi_config.sta.ssid[copy_index++] = *ssid++;
	}
	copy_index = 0;
	while (*password) {
		wifi_config.sta.password[copy_index++] = *password++;
	}

	// We have to set the mode BEFORE setting the config
	PASS_ERROR(esp_wifi_set_mode(WIFI_MODE_STA), "Unable to set WiFi mode");
	PASS_ERROR(esp_wifi_set_config(WIFI_IF_STA, &wifi_config),
			   "Unable to configure WiFi");
	PASS_ERROR(
		esp_wifi_set_ps(WIFI_PS_NONE),
		"Unable to set Power Saving mode"); // TODO: figure out which PS we need
	LOG("WiFi configured");

	PASS_ERROR(esp_wifi_start(), "Unable to begin WiFi");
	LOG("WiFi started");

	return ESP_OK;
}

esp_err_t wifi_init(esp_err_t (*callback)(void)) {
	wifi_connected_callback = callback;

	PASS_ERROR(esp_netif_init(), "Failed to init Network Stack");
	PASS_ERROR(esp_event_loop_create_default(), "Failed to create event loop");
	esp_netif_create_default_wifi_sta();

	// Setup the init config
	wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
	init_config.nvs_enable = false;
	init_config.nano_enable = false;
	PASS_ERROR(esp_wifi_init(&init_config), "Failed to init WiFi");
	LOG("WiFi succesfully initialised");

	// Register events
	esp_event_handler_instance_t event_wifi_instance, event_ip_instance;
	esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
										&wifi_event_handler, NULL,
										&event_wifi_instance);
	esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID,
										&ip_event_handler, NULL,
										&event_ip_instance);

	return ESP_OK;
}
