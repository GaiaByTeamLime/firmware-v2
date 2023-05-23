
#include <stdio.h>

#include "ndef.h"

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
				"%c ", represent_byte(buffer[index])
			);
		}
		line[line_width - 2] = '|';
		LOG("| %02x %s", address++, line);
	}
	LOG("+----+-------------+---------+");
}

esp_err_t ndef_full_scan(spi_device_handle_t* handle, tag_data_t* tag) {
	for (uint8_t sectors = 0; sectors < 9; sectors++) {
		PASS_ERROR(rfid_read_mifare_tag(
			handle, 4 + sectors * 4, tag->raw_data + sectors * 16, 16
		), "Failed reading sector");
	}
	
	return ESP_OK;
}

tag_data_t ndef_create_type() {
	uint8_t* data_ptr = (uint8_t*)malloc(MAX_BYTE_COUNT);
	return (tag_data_t){
		.raw_data = data_ptr,
		.raw_data_length = MAX_BYTE_COUNT,
		.pointer = data_ptr,
		.records = NULL,
		.record_count = 0
	};
}

void ndef_destroy_type(tag_data_t* tag) {
	if (tag->raw_data != NULL) {
		free(tag->raw_data);
	}
	if (tag->records != NULL) {
		free(tag->records);
	}
	tag->record_count = 0;
	tag->raw_data_length = 0;
	tag->raw_data = NULL;
	tag->records = NULL;
	tag->pointer = NULL;
}

esp_err_t test(spi_device_handle_t* handle) {
	uint8_t raw_buffer[144] = {0};
	ndef_record_t record_buffer[2] = {0};
	tag_data_t data = ndef_create_type(raw_buffer, record_buffer);

	ndef_full_scan(handle, &data);
	return ESP_OK;
}

