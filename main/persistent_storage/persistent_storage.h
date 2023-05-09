#ifndef __PERSISTENT_STORAGE_HEADER__
#define __PERSISTENT_STORAGE_HEADER__

#include "../prelude.h"

#include <stdint.h>
#include <string.h>

#include <esp_err.h>
#include <nvs_flash.h>

#define NVS_NAMESPACE "PERSIST_STORE"

#define WIFI_SSID_KEY "WIFI_SSID"
#define WIFI_PASSWORD_KEY "WIFI_PASSWORD"

#define WIFI_SSID_MAX_LENGTH 32
#define WIFI_SSID_CHAR_BUFFER_LENGTH WIFI_SSID_MAX_LENGTH + 1
#define WIFI_PASSWORD_MAX_LENGTH 64
#define WIFI_PASSWORD_CHAR_BUFFER_LENGTH WIFI_PASSWORD_MAX_LENGTH + 1

/**
 * Initializes the Persistent Storage
 */
esp_err_t persistent_storage_init();

/**
 * Wrapper function to set strings to Persistent Storage
 *
 * @param nvs Pointer of the NVS handle
 * @param key The NVS key to save the string to
 * @param value The value of the string
 * @param max_string_length The maximum length of the string. This will be the
 * character array buffer size
 */
esp_err_t persistent_storage_set_str(
	nvs_handle_t* nvs, const char* key, const char* value,
	size_t max_string_length
);

/**
 * Store the WiFi credentials in Persistent Storage
 *
 * @param ssid The WiFi SSID
 * @param password The WiFi password
 */
esp_err_t persistent_storage_set_wifi(const char* ssid, const char* password);

/**
 * Get WiFi credentials from Persistent Storage
 *
 * @param ssid A character buffer in which the WiFi SSID will be stored. It
 * needs to be at least WIFI_SSID_CHAR_BUFFER_LENGTH long
 * @param password A character buffer in which the WiFi Password will be stored.
 * It needs to be at least WIFI_PASSWORD_CHAR_BUFFER_LENGTH long
 */
esp_err_t persistent_storage_get_wifi(char* ssid, char* password);

#endif
