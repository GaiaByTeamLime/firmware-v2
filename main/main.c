#include "prelude.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_heap_caps.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <string.h>

#include "adc/adc.h"
#include "ndef/ndef.h"
#include "persistent_storage/persistent_storage.h"
#include "picc/picc.h"
#include "rfid/rfid.h"
#include "rfid/rfid_pcd_register_types.h"
#include "spi/spi.h"
#include "wifi/wifi.h"

char represent_byte(uint8_t byte) {
	if ((byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'z') ||
		(byte >= 'A' && byte <= 'Z')) {
		return (char)byte;
	}
	return '.';
}

void print_buffer(uint8_t* buffer, uint16_t size, uint8_t address) {
	// Output the data
	const uint8_t width = 4;
	const uint8_t left_padding = 2;
	const uint8_t right_padding = 2;
	const uint8_t middle_left = 1;
	const uint8_t middle_right = 2;
	const uint8_t line_width = left_padding + width * 3 - 1 +
							   (middle_left + middle_right) + width * 2 +
							   right_padding;
	const uint8_t middle_start = left_padding + width * 3 - 1;
	const uint8_t block_data = 16;

	char line[line_width];
	for (uint8_t x = 0; x < line_width; x++) {
		line[x] = '#';
	}
	line[0] = '|';
	line[1] = ' ';

	line[middle_start] = ' ';
	line[middle_start + 1] = '|';
	line[middle_start + 2] = ' ';

	line[line_width - 3] = ' ';
	line[line_width - 2] = '|';
	line[line_width - 1] = '\0';

	LOG("+----+-------------+---------+");
	LOG("|ADDR| BYTES       | CHARS   |");
	for (uint16_t y = 0; y < size; y += width) {
		if (y % block_data == 0) {
			LOG("+----+-------------+---------+");
		}

		for (uint8_t x = 0; x < width; x++) {
			uint8_t index = y + x;
			sprintf(line + 3 * x + left_padding, "%02x ", buffer[index]);
		}
		line[middle_start + 1] = '|';
		for (uint8_t x = 0; x < width; x++) {
			uint8_t index = y + x;
			sprintf(
				middle_start + (middle_left + middle_right) + line + 2 * x,
				"%c ",
				represent_byte(buffer[index])
			);
		}
		line[line_width - 2] = '|';
		LOG("| %02x %s", address++, line);
	}
	LOG("+----+-------------+---------+");
}

/**
 * A simple rewrite of memcpy_s.
 *
 * It copies over bytes from one byte array to another byte array.
 *
 * This function takes in two lengths, the maximum amount of bytes copied over
 * is the minimum of both lengths.
 *
 * @param dest The destination pointer
 * @param dest_count How many bytes it should write too
 * @param src The source pointer
 * @param src_count How many bytes it should copy from
 */
void byte_copy(
	uint8_t* dest, uint8_t dest_count, uint8_t* src, uint8_t src_count
) {
	uint8_t max_copy = dest_count < src_count ? dest_count : src_count;
	for (uint8_t index = 0; index < max_copy; index++) {
		dest[index] = src[index];
	}
}

esp_err_t setup(spi_device_handle_t* rfid_spi_handle) {
	PASS_ERROR(adc_init(), "ADC1 Init Error");
	PASS_ERROR(
		persistent_storage_init(), "Could not initialize persistent storage"
	);

	PASS_ERROR(spi2_init(), "Could not initialize SPI2 Host");
	PASS_ERROR(rfid_init(rfid_spi_handle), "Could not add RFID to SPI Host");

	PASS_ERROR(wifi_init(&on_wifi_connect), "Unable to init WiFi");

	return ESP_OK;
}

void on_wifi_connect(void) {
	connection_data_t connection_data;
	if (persistent_storage_get_connection_data(&connection_data) != ESP_OK) {
		// delete wifi credentials
		persistent_storage_erase();
		// deep sleep (forever)
		esp_deep_sleep_start();
	}

	uint32_t sensor_values[] = {2, 69, 420, 1, 2, 3, 4};
	wifi_send_data_to_server(&connection_data, sensor_values);
}

esp_err_t get_and_store_credentials(spi_device_handle_t* handle) {
	PASS_ERROR(rfid_wakeup_mifare_tag(handle), "Unable to wake-up MiFare tag");
	tag_data_t tag = ndef_create_type();
	PASS_ERROR(ndef_full_scan(handle, &tag), "Unable to scan MiFare tag");

	connection_data_t connection_data;
	picc_get_ssid(&tag, connection_data.ssid);
	picc_get_token(&tag, connection_data.token);
	picc_get_password(&tag, connection_data.password);
	picc_get_sid(&tag, connection_data.sid);

	// Destroy the tag
	ndef_destroy_type(&tag);

	PASS_ERROR(
		persistent_storage_set_connection_data(&connection_data),
		"Unable to store into persistent storage"
	);
	return ESP_OK;
}

void app_main(void) {
	// setup
	spi_device_handle_t rfid_handle = {0};
	setup(&rfid_handle);

	// get wifi credentials
	get_and_store_credentials(&rfid_handle);

	connection_data_t connection_data;
	persistent_storage_get_connection_data(&connection_data);

	wifi_start(connection_data.ssid, connection_data.password);

	// char ssid[WIFI_SSID_CHAR_BUFFER_LENGTH];
	// char password[WIFI_PASSWORD_CHAR_BUFFER_LENGTH];
	// esp_err_t result = persistent_storage_get_wifi(&ssid, &password);

	// if (result != ESP_OK || /* wifi credentials are not pressent */) {
	// 	{
	// 		// read nfc tag
	// 		if (/* wifi credentials are finaly present*/) {
	// 			// save wifi credentials
	// 			persistent_storage_set_wifi(&ssid, &password);
	// 		} else {
	// 			// deep sleep (forever)
	// 			esp_deep_sleep_start();
	// 		}
	// 	}
	// }

	// // connect to wifi
	// wifi_start(&ssid, &password);
}
