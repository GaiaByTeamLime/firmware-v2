#include "persistent_storage.h"

esp_err_t persistent_storage_init() { return nvs_flash_init(); }

esp_err_t persistent_storage_set_str(
	nvs_handle_t* nvs,
	const char* key,
	const char* value,
	size_t max_string_length
) {
	char value_buffer[max_string_length + 1]; // +1 for the NULL terminator
	uint16_t value_length = strlen(value);
	strncpy(value_buffer, value, max_string_length);
	value_buffer[value_length] = '\0';

	PASS_ERROR(nvs_set_str(*nvs, key, value), "Failed to write string to NVS");

	return ESP_OK;
}

esp_err_t persistent_storage_set_connection_data(
	connection_data_t* connection_data
) {
	nvs_handle_t nvs;
	PASS_ERROR(
		nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs),
		"Failed to open NVS storage"
	);
	LOG("NVS Open");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs, WIFI_SSID_KEY, connection_data->ssid, WIFI_SSID_MAX_LENGTH
		),
		"Failed to write WiFi SSID to storage"
	);
	LOG("NVS Set WiFi SSID");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs,
			WIFI_PASSWORD_KEY,
			connection_data->password,
			WIFI_PASSWORD_MAX_LENGTH
		),
		"Failed to write WiFi password to storage"
	);
	LOG("NVS Set WiFi password");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs, SENSOR_ID_KEY, connection_data->sid, SENSOR_ID_LENGTH
		),
		"Failed to write sensor ID to storage"
	);
	LOG("NVS Set sensor ID");

	PASS_ERROR(
		persistent_storage_set_str(
			&nvs, SENSOR_TOKEN_KEY, connection_data->token, SENSOR_TOKEN_LENGTH
		),
		"Failed to write sensor token to storage"
	);
	LOG("NVS Set sensor token");

	PASS_ERROR(nvs_commit(nvs), "Failed to commit NVS");
	LOG("NVS Commit");

	nvs_close(nvs);
	LOG("NVS Close");

	return ESP_OK;
}

esp_err_t persistent_storage_get_connection_data(
	connection_data_t* connection_data
) {
	nvs_handle_t nvs;
	PASS_ERROR(
		nvs_open(NVS_NAMESPACE, NVS_READONLY, &nvs),
		"Failed to open NVS storage"
	);
	LOG("NVS Open");

	size_t max_length = WIFI_SSID_MAX_LENGTH;
	PASS_ERROR(
		nvs_get_str(nvs, WIFI_SSID_KEY, connection_data->ssid, &max_length),
		"Failed to get WiFi SSID from NVS storage"
	);
	LOG("NVS Get WiFi SSID");

	max_length = WIFI_PASSWORD_MAX_LENGTH;
	PASS_ERROR(
		nvs_get_str(
			nvs, WIFI_PASSWORD_KEY, connection_data->password, &max_length
		),
		"Failed to get WiFi password from NVS storage"
	);
	LOG("NVS Get WiFi password");

	max_length = SENSOR_ID_LENGTH;
	PASS_ERROR(
		nvs_get_str(nvs, SENSOR_ID_KEY, connection_data->sid, &max_length),
		"Failed to get sensor ID from NVS storage"
	);
	LOG("NVS Get sensor ID");

	max_length = SENSOR_TOKEN_LENGTH;
	PASS_ERROR(
		nvs_get_str(nvs, SENSOR_TOKEN_KEY, connection_data->token, &max_length),
		"Failed to get sensor token from NVS storage"
	);
	LOG("NVS Get sensor token");

	return ESP_OK;
}

esp_err_t persistent_storage_erase() { return nvs_flash_erase(); }
