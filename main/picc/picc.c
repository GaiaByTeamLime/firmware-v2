
#include <esp_err.h>

#include "ndef.h"
#include "picc.h"

void picc_parse(uint8_t* cursor, char* buffer, uint8_t max_write) {
	uint8_t write_count = 0;
	while (*cursor != PICC_END_OF_STRING_CHAR && write_count < max_write) {
		*(buffer++) = *(cursor++);
		write_count++;
	}
	*buffer = '\0';
}

void picc_get_ssid(tag_data_t* tag, char* ssid) {
	uint8_t* cursor = tag->raw_data;
	cursor += SKIP_BYTES + TLV_PREAMBLE + NDEF_TYPE_LENGTH + PAYLOAD_PREAMBLE;
	picc_parse(cursor, ssid, PICC_SSID_LENGTH);
}

void picc_get_password(tag_data_t* tag, char* password) {
	uint8_t* cursor = tag->raw_data;
	cursor += SKIP_BYTES + TLV_PREAMBLE + NDEF_TYPE_LENGTH + PAYLOAD_PREAMBLE +
			  PICC_SSID_LENGTH;
	picc_parse(cursor, password, PICC_PASS_LENGTH);
}

void picc_get_sid(tag_data_t* tag, char* url) {
	uint8_t* cursor = tag->raw_data;
	cursor += SKIP_BYTES + TLV_PREAMBLE + NDEF_TYPE_LENGTH + PAYLOAD_PREAMBLE +
			  PICC_SSID_LENGTH + PICC_PASS_LENGTH;
	picc_parse(cursor, url, SENSOR_ID_LENGTH);
}

void picc_get_token(tag_data_t* tag, char* token) {
	uint8_t* cursor = tag->raw_data;
	cursor += SKIP_BYTES + TLV_PREAMBLE + NDEF_TYPE_LENGTH + PAYLOAD_PREAMBLE +
			  PICC_SSID_LENGTH + PICC_PASS_LENGTH + PICC_SID_LENGTH;
	picc_parse(cursor, token, SENSOR_TOKEN_LENGTH);
}
