
#include <stdio.h>
#include <stdlib.h>

#include "ndef.h"
#include "prelude.h"

#define USER_DATA_BEGIN 4
#define BYTES_PER_SECTOR 4
#define MAX_RETURN_BYTES 16

esp_err_t ndef_full_scan(spi_device_handle_t* handle, tag_data_t* tag) {
	for (uint8_t sectors = 0;
		 sectors < (MAX_BYTE_COUNT / MAX_RETURN_BYTES);
		 sectors++) {
		PASS_ERROR(
			rfid_read_mifare_tag(
				handle,
				USER_DATA_BEGIN + sectors * BYTES_PER_SECTOR,
				tag->raw_data + sectors * MAX_RETURN_BYTES,
				MAX_RETURN_BYTES
			),
			"Failed reading sector"
		);
	}
	LOG("NDEF Scanned entire PICC device");

	return ESP_OK;
}

tag_data_t ndef_create_type() {
	uint8_t* data_ptr = (uint8_t*)malloc(MAX_BYTE_COUNT);
	// Clear the array
	for (uint16_t index = 0; index < MAX_BYTE_COUNT; index++) {
		data_ptr[index] = 0;
	}
	return (tag_data_t
	){.raw_data = data_ptr,
	  .raw_data_length = MAX_BYTE_COUNT,
	  .pointer = data_ptr,
	  .records = {},
	  .record_count = 0};
}

void ndef_destroy_type(tag_data_t* tag) {
	if (tag->raw_data != NULL) {
		free(tag->raw_data);
	}
	for (uint8_t record_index = 0; record_index < tag->record_count;
		 record_index++) {
		free(tag->records[record_index].payload);
	}
	tag->record_count = 0;
	tag->raw_data_length = 0;
	tag->raw_data = NULL;
	tag->pointer = NULL;
}

uint8_t ndef_move_to_nearest_tlv(tag_data_t* tag) {
	// If we're still at the start of the data tag, then skip the first few
	// bytes
	if (tag->pointer == tag->raw_data) {
		tag->pointer += SKIP_BYTES;
	}

	uint8_t tlv_type = TLV_NULL;
	while (tlv_type == TLV_NULL) {
		tlv_type = *(tag->pointer++);
	}
	return *(tag->pointer++);
}

bool ndef_is_end_of_tlv(tag_data_t* tag) {
	return *(tag->pointer) == TLV_TERMINATE;
}

esp_err_t ndef_parse_record(tag_data_t* tag) {
	uint8_t record_header = *(tag->pointer++);
	uint8_t type_length = *(tag->pointer++);
	uint8_t payload_length = *(tag->pointer++);
	uint8_t type_field = *(tag->pointer++);

	// We only support the Well-Known, Text mode, no other
	if ((record_header & 0x03) != RECORD_TNF_WELL_KNOWN) {
		ELOG("Record is the wrong type");
		return ESP_ERR_NOT_SUPPORTED;
	}
	if (type_length != NDEF_TYPE_LENGTH || type_field != NDEF_TYPE_FIELD) {
		ELOG(
			"The type length (%d) or the typefield (%02x) is invalid",
			type_length,
			type_field
		);
		return ESP_ERR_NOT_SUPPORTED;
	}

	// The payload preamble
	// We have to check if this is valid before continueing
	if (*(tag->pointer++) != PAYLOAD_STATUS) {
		ELOG("Record has invalid payload status, must be 0x02");
		return ESP_ERR_INVALID_STATE;
	}
	if (*(tag->pointer++) != PAYLOAD_IANA_MSB) {
		ELOG("Record has invalid IANA language, must be US-ASCII. (encoded as "
			 "`en`)");
		return ESP_ERR_INVALID_STATE;
	}
	if (*(tag->pointer++) != PAYLOAD_IANA_LSB) {
		ELOG("Record has invalid IANA language, must be US-ASCII. (encoded as "
			 "`en`)");
		return ESP_ERR_INVALID_STATE;
	}

	// Re-allocate the memory, as we need to increase the array
	tag->record_count++;
	LOG("NDEF Parsed %dth NDEF Record", tag->record_count);

	ndef_record_t* record = tag->records + tag->record_count - 1;
	record->payload = malloc(payload_length - PAYLOAD_PREAMBLE);
	record->payload_size = payload_length - PAYLOAD_PREAMBLE;

	for (uint8_t index = PAYLOAD_PREAMBLE; index < payload_length; index++) {
		record->payload[index - PAYLOAD_PREAMBLE] = *(tag->pointer++);
	}

	return ESP_OK;
}

esp_err_t ndef_extract_all_records(
	spi_device_handle_t* handle, tag_data_t* tag
) {
	// Scan the entire tag
	PASS_ERROR(ndef_full_scan(handle, tag), "Unable to scan entire tag");
	// Read all read all NDEF records
	ndef_move_to_nearest_tlv(tag);
	while ((!ndef_is_end_of_tlv(tag)) && (tag->record_count < MAX_RECORD_COUNT)
	) {
		PASS_ERROR(ndef_parse_record(tag), "Unable to find NDEF record");
	}
	LOG("NDEF Exacted All NDEF Records");
	return ESP_OK;
}
