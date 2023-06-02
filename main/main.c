#include "prelude.h"

#include "adc/adc.h"
#include "ndef/ndef.h"
#include "persistent_storage/persistent_storage.h"
#include "rfid/rfid.h"
#include "rfid/rfid_pcd_register_types.h"
#include "spi/spi.h"
#include "wifi/wifi.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>
#include <esp_sleep.h>
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

bool connected = false;

esp_err_t on_wifi_connect(void) {
	// collect sensordata
	// send sensordata
	// close connection
	connected = true;
	wifi_stop();
	return ESP_OK;
}

void on_wifi_disconnect(void) {
	if (!connected) {
		// delete wifi credentials
		persistent_storage_set_wifi(NULL, NULL);
		// deep sleep (forever)
		esp_deep_sleep_start();
	} else {
		// deep sleep (60 minutes)
		// esp_deep_sleep();
	}
	return ESP_OK;
}

esp_err_t setup(spi_device_handle_t* rfid_spi_handle) {
	PASS_ERROR(adc_init(), "ADC1 Init Error");
	PASS_ERROR(
		persistent_storage_init(), "Could not initialize persistent storage"
	);

	PASS_ERROR(spi2_init(), "Could not initialize SPI2 Host");
	PASS_ERROR(rfid_init(rfid_spi_handle), "Could not add RFID to SPI Host");

	PASS_ERROR(
		wifi_init(&on_wifi_connect, &on_wifi_disconnect),
		"Could not initialize wifi"
	);

	return ESP_OK;
}

void app_main(void) {
	// setup
	spi_device_handle_t rfid_handle = {0};
	setup(&rfid_handle);

	// get wifi credentials
	char ssid[WIFI_SSID_CHAR_BUFFER_LENGTH];
	char password[WIFI_PASSWORD_CHAR_BUFFER_LENGTH];
	esp_err_t result = persistent_storage_get_wifi(&ssid, &password);

	if (result != ESP_OK || /* wifi credentials are not pressent */) {
		{
			// read nfc tag
			if (/* wifi credentials are finaly present*/) {
				// save wifi credentials
				persistent_storage_set_wifi(&ssid, &password);
			} else {
				// deep sleep (forever)
				esp_deep_sleep_start();
			}
		}
	}

	// connect to wifi
	wifi_start(&ssid, &password);
}
