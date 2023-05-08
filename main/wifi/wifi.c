
#include "../prelude.h"

#include <esp_event.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <nvs_flash.h>

#include "esp_err.h"
#include "esp_wifi_types.h"
#include "wifi.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAILED_BIT BIT1

static EventGroupHandle_t wifi_event_group;

static void ip_event_handler(void *arg, esp_event_base_t event_base,
							 int32_t event_id, void *event_data) {
	// We got a connection!
	if (event_id == IP_EVENT_STA_GOT_IP) {
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data) {
	// Just keep retrying on disconnects
	if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
		esp_wifi_connect();
	}
}

esp_err_t wifi_init(const char *ssid, const char *password) {
	wifi_event_group = xEventGroupCreate();

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

	// Create a config struct
	wifi_config_t wifi_config = {.sta = {
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

	PASS_ERROR(esp_wifi_connect(), "Unable to connect to AP");
	LOG("WiFi connecting");

	EventBits_t bits = xEventGroupWaitBits(wifi_event_group,
										   WIFI_CONNECTED_BIT | WIFI_FAILED_BIT,
										   pdFALSE, pdFALSE, portMAX_DELAY);

	if (bits & WIFI_CONNECTED_BIT) {
		LOG("Great success! We connected!!");
	} else if (bits & WIFI_FAILED_BIT) {
		LOG("DEATH.");
	} else {
		LOG("DEATH AGAIN.");
	}

	return ESP_OK;
}
