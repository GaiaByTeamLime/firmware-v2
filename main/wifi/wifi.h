#ifndef __WIFI_HEADER__
#define __WIFI_HEADER__

#include <esp_err.h>

#include "prelude.h"

#define MAX_SSID_LENGTH 32
#define MAX_PASSWORD_LENGTH 64

#define SENSOR_DATA_FIELD_COUNT 4
#define JSON_WRAPPER_BYTES 2
#define BYTES_PER_NUMBER 10
#define BYTES_PER_FIELD (4 + BYTES_PER_NUMBER + 1)
#define SERIALISED_DATA_MAX_BYTES \
	(JSON_WRAPPER_BYTES + SENSOR_DATA_FIELD_COUNT * BYTES_PER_FIELD)

#define BASE_DATABASE_URL "https://temporal.dev.gaiaplant.app/logv2/"
#define BASE_DATABASE_URL_LENGTH 41 // This is without the null-byte
#define HEADER_TEXT_LENGTH 7

/**
 * Setup the entire WiFi driver, register a callback function for
 * when the device connects to a network.
 * At the current moment, we only support WPA2-PSK encryption method
 *
 * @param success The callback function to be invoked once the device connected
 * to a network* @ return Any error the WiFi driver may have encountered
 */
esp_err_t wifi_init(void (*success)(void));

/**
 * Send sensor data to the sensor database.
 *
 * @see wifi_serialise_data
 *
 * @param sensor_values The sensor values, the length of this array must equal
 * `SENSOR_DATA_FIELD_COUNT`
 */
esp_err_t wifi_send_data_to_server(uint32_t* sensor_values);

/**
 * Start & connect to the WiFi network
 *
 * Connection to the network does NOT happen immediately, this function only
 * invokes the start of the network. The event listener then connects to the
 * network
 *
 * @param data The configuration data used to connect to the server
 * @return Any error the WiFi driver may have encountered
 */
esp_err_t wifi_start(connection_data_t data);

/**
 * Serialise sensor data into a compact JSON data format
 *
 * The sensor data is serialised according to the following format, it also
 * shows which value is expected where in the `sensor_data` array
 *
 * | Index in array | Serialised field | Name | Expected Data |
 * |----------------|------------------|------|---------------|
 * | 1 | f | Firmware Version | 2 |
 * | 2 | s | Soil Humidity | x |
 * | 3 | i | Illumination | x |
 * | 4 | v | Voltage | x |
 *
 * x = the value of the sensor
 *
 * @note No lengths have to be given, as the data array should always be
 * SENSOR_DATA_FIELD_COUNT
 * @note The same applies for the return value, the length of the buffer MUST be
 * AT LEAST SERIALISED_DATA_MAX_BYTES
 * @note If you want to add more fields, you have to change the value of
 * SENSOR_DATA_FIELD_COUNT and the fields char array in this function. Note that
 * the keys are in the right order.
 *
 * @param sensor_data A pointer to an array of sensor data
 * @param output A pointer to an output char buffer, the serialised data gets
 * written to here
 * @returns The amount of bytes written to the sensor data array, INCLUDING the
 * NULL-byte
 */
uint32_t wifi_serialise_data(uint32_t* sensor_data, char* output);

#endif
