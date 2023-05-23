#ifndef __NDEF_H__
#define __NDEF_H__

#include "rfid.h"

typedef struct {
	uint8_t* payload;
	uint8_t payload_size;
} ndef_record_t;

typedef struct {
	uint8_t* raw_data;
	const uint16_t raw_data_length;
	uint8_t* pointer;
	ndef_record_t* records;
	uint8_t record_count;
} tag_data_t;

/**
 * We skip a few bytes at the start of the tag, this is because the data isn't
 * significant to us, it is used to store LOCK data, but as we do not lock
 * anything, this is useless data and not worth investigating. For those who are
 * curious, the byte layout is:
 *  - The type? For the MiFare Ultralight C3 this value is 0x01, which I
 * *assume* means `Lock Control TLV`
 *  - ???
 *  - ???
 *  - How many lock bits there are
 *  - ???
 */
#define SKIP_BYTES 5

#define TLV_NDEF_MESSAGE 0x03
#define TLV_TERMINATE 0xfe
#define TLV_NULL 0x00

#define RECORD_TNF_EMPTY_RECORD 0x00
#define RECORD_TNF_WELL_KNOWN 0x01
#define RECORD_TNF_MIME_TYPE 0x02
#define RECORD_TNF_ABSOLUTE_URL 0x03
#define RECORD_TNF_EXTERNAL 0x04
#define RECORD_TNF_UNKNOWN 0x05
#define RECORD_TNF_UNCHANGED 0x06

esp_err_t test(spi_device_handle_t* handle);

#endif
