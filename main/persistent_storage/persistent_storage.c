#include "persistent_storage.h"

esp_err_t persistent_storage_init() { return nvs_flash_init(); }

esp_err_t persistent_storage_set_str(
	nvs_handle_t* nvs, const char* key, const char* value,
	size_t max_string_length
) {
	char value_buffer[max_string_length + 1]; // +1 for the NULL terminator
	uint16_t value_length = strlen(value);
	strncpy(value_buffer, value, max_string_length);
	value_buffer[value_length] = '\0';

	PASS_ERROR(nvs_set_str(*nvs, key, value), "Failed to write string to NVS");
	LOG("NVS Set string");

	return ESP_OK;
}

esp_err_t persistent_storage_set_wifi(const char* ssid, const char* password) {
	nvs_handle_t nvs;
	PASS_ERROR(
		nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs),
		"Failed to open NVS storage"
	);
	LOG("NVS Open");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs, WIFI_SSID_KEY, ssid, WIFI_SSID_MAX_LENGTH
		),
		"Failed to write WiFi SSID to storage"
	);
	LOG("NVS set_str");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs, WIFI_PASSWORD_KEY, password, WIFI_PASSWORD_MAX_LENGTH
		),
		"Failed to write WiFi password to"
	);
	LOG("NVS set_str");

	PASS_ERROR(nvs_commit(nvs), "Failed to commit NVS");
	LOG("NVS Commit");

	nvs_close(nvs);
	LOG("NVS Close");

	return ESP_OK;
}

esp_err_t persistent_storage_get_wifi(char* ssid, char* password) {
	nvs_handle_t nvs;
	PASS_ERROR(
		nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs),
		"Failed to open NVS storage"
	);
	LOG("NVS Open");

	size_t max_length = WIFI_SSID_CHAR_BUFFER_LENGTH;
	PASS_ERROR(
		nvs_get_str(nvs, WIFI_SSID_KEY, ssid, &max_length),
		"Failed to get WiFi SSID from NVS storage"
	);
	LOG("NVS get_str");

	max_length = WIFI_PASSWORD_CHAR_BUFFER_LENGTH;
	PASS_ERROR(
		nvs_get_str(nvs, WIFI_PASSWORD_KEY, password, &max_length),
		"Failed to get WiFi Password from NVS storage"
	);
	LOG("NVS get_str");

	nvs_close(nvs);
	LOG("NVS Close");

	return ESP_OK;
}
