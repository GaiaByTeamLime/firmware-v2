#include "prelude.h"

#include "adc/adc.h"
#include "persistent_storage/persistent_storage.h"
#include "rfid/rfid.h"
#include "rfid/rfid_pcd_register_types.h"
#include "spi/spi.h"
// #include "wifi/wifi.h"
#include "ndef/ndef.h"
#include "sensors/sensors.h"
#include "sensors/sensors.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

char represent_byte(uint8_t byte) {
	if ((byte >= '0' && byte <= '9') || (byte >= 'a' && byte <= 'z') ||
		(byte >= 'A' && byte <= 'Z')) {
		return (char)byte;
	}
	return '.';
}

void print_buffer(uint8_t* buffer, uint8_t size, uint8_t address) {
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
	for (uint8_t y = 0; y < size; y += width) {
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

esp_err_t on_wifi_connect(void) {
	LOG("Yay we connected!");
	return ESP_OK;
}

esp_err_t setup(spi_device_handle_t* rfid_spi_handle) {
	PASS_ERROR(adc_init(), "ADC1 Init Error");
	PASS_ERROR(
		persistent_storage_init(), "Could not initialize persistent storage"
	);

	PASS_ERROR(spi2_init(), "Could not initialize SPI2 Host");
	PASS_ERROR(rfid_init(rfid_spi_handle), "Could not add RFID to SPI Host");
	PASS_ERROR(sensors_init(), "Could not initialize sensors");

	return ESP_OK;
}

void app_main(void) {
	spi_device_handle_t rfid_handle = {0};
	setup(&rfid_handle);

	while (true) {
		pull_latest_data();

		measure_soil_capacity();
		measure_ldr();
		measure_battery_voltage();
		vTaskDelay(pdMS_TO_TICKS(250));
	}

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

	// Read actual data from the tag
	// rfid_read_mifare_tag(&rfid_handle, 4 + 4, buffer, 16);

	// Output the data
	// LOG("SUCCES:");
	// for (uint8_t i = 0; i < 16; i++) {
	// 	LOG("\t%x (%c)", buffer[i], buffer[i]);
	// }
	// LOG("END");

	// while (1) {
	// 	pull_latest_data();
	// 	uint32_t adc_data;
	// 	message = get_adc_data(ADC1_LDR, &adc_data);
	// 	if (message == ESP_OK) {
	// 		LOG("LDR Data: %lu", adc_data);
	// 	}
	// 	LOG("Test! %d", index++);
	// 	vTaskDelay(1000 / portTICK_PERIOD_MS);
	// }

	// wifi_init(&on_wifi_connect);
	// wifi_start("TestSpot", "baguette");
}
