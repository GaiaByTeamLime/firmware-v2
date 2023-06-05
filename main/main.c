#include "prelude.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_heap_caps.h>
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
#include "sensors/sensors.h"

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

void on_wifi_connect(void) {
	connection_data_t connection_data;
	if (persistent_storage_get_connection_data(&connection_data) != ESP_OK) {
		// TODO: Enter sleep mode
	}

	uint32_t sensor_values[] = {2, 69, 420, 1, 2, 3, 4};
	wifi_send_data_to_server(&connection_data, sensor_values);
}

esp_err_t setup(spi_device_handle_t* rfid_spi_handle) {
	PASS_ERROR(adc_init(), "ADC1 Init Error");
	PASS_ERROR(
		persistent_storage_init(), "Could not initialize persistent storage"
	);

	PASS_ERROR(spi2_init(), "Could not initialize SPI2 Host");
	PASS_ERROR(rfid_init(rfid_spi_handle), "Could not add RFID to SPI Host");
	PASS_ERROR(sensors_init(), "Could not initialize sensors");

	PASS_ERROR(wifi_init(&on_wifi_connect), "Unable to init WiFi");

	return ESP_OK;
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
	spi_device_handle_t rfid_handle = {0};
	setup(&rfid_handle);

	get_and_store_credentials(&rfid_handle);

	connection_data_t connection_data;
	persistent_storage_get_connection_data(&connection_data);
	// // Wake up the rfid reader
	// rfid_wakeup_mifare_tag(&rfid_handle);

	// tag_data_t tag = ndef_create_type();

	// ndef_extract_all_records(&rfid_handle, &tag);

	// print_buffer(tag.raw_data, tag.raw_data_length, 4);

	// // Print out all records
	// LOG("Record count: %d", tag.record_count);
	// for (uint8_t record_index = 0; record_index < tag.record_count;
	// 	 record_index++) {
	// 	ndef_record_t record = tag.records[record_index];
	// 	LOG("Record (payload length=%d):", record.payload_size);
	// 	for (uint8_t byte_index = 0; byte_index < record.payload_size;
	// 		 byte_index++) {
	// 		uint8_t data = record.payload[byte_index];
	// 		LOG("\t0x%02x = %c", data, represent_byte(data));
	// 	}
	// }
	// LOG("End of records");

	// ndef_destroy_type(&tag);

	// adc_init();
	// pull_latest_data();
	// uint32_t data = 0;
	// get_adc_data(ADC1_LDR, &data);
	// LOG("%d", (int)data);

	wifi_start(connection_data.ssid, connection_data.password);

	// char ssid[MAX_SSID_LENGTH] = {0};
	// char password[MAX_PASSWORD_LENGTH] = {0};
	// esp_err_t cred_err = get_and_store_credentials(&rfid_handle);
	// if (cred_err != ESP_OK) {
	// 	ELOG("Unable to retrieve store credentials in NVS");
	// }
	// esp_err_t persistent_err = persistent_storage_get_wifi(ssid, password);
	// if (persistent_err != ESP_OK) {
	// 	ELOG("Unable to retrieve credentials from NVS");
	// }
	//
	// // Connect to WiFi
	// wifi_start(ssid, password);
	//
	// while (1) {
	// 	pull_latest_data();
	// 	uint32_t adc_data;
	// 	esp_err_t error_code = get_adc_data(ADC1_LDR, &adc_data);
	// 	if (error_code == ESP_OK) {
	// 		LOG("LDR data: %" PRIu32, adc_data);
	// 	}
	// 	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// }
}
