#ifndef __NDEF_H__
#define __NDEF_H__

#include "rfid.h"

#define MAX_RECORD_COUNT 4

typedef struct ndef_record {
	uint8_t* payload;
	uint8_t payload_size;
} ndef_record_t;

typedef struct tag_data {
	uint8_t* raw_data;
	uint16_t raw_data_length;
	uint8_t* pointer;
	ndef_record_t records[MAX_RECORD_COUNT];
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

/**
 * How many bytes there are in the tag. Bad things happen if this value isn't
 * set properly. For the MiFare Ultralight C3, the proper value is 144
 */
#define MAX_BYTE_COUNT 144

/**
 * How many bytes there are in an TLV message
 */
#define TLV_PREAMBLE 5

#define TLV_NDEF_MESSAGE 0x03
#define TLV_TERMINATE 0xfe
#define TLV_NULL 0x00

#define NDEF_TYPE_LENGTH 1
#define NDEF_TYPE_FIELD 'T'

/**
 * How many bytes there are before the NDEF record payload
 */
#define PAYLOAD_PREAMBLE 3
/**
 * The first byte in the NDEF record preamble, this byte contains data about the
 * status of the NDEF record, however, we only support one kind of record,
 * therefore this will always be the same.
 */
#define PAYLOAD_STATUS 0x02
/**
 * The 2nd byte in the NDEF record preamble, only support ANSII-US encoding, the
 * signiture of this is 'en'.
 */
#define PAYLOAD_IANA_MSB 'e'
/**
 * The 2nd byte in the NDEF record preamble, only support ANSII-US encoding, the
 * signiture of this is 'en'.
 */
#define PAYLOAD_IANA_LSB 'n'

#define RECORD_TNF_EMPTY_RECORD 0x00
#define RECORD_TNF_WELL_KNOWN 0x01
#define RECORD_TNF_MIME_TYPE 0x02
#define RECORD_TNF_ABSOLUTE_URL 0x03
#define RECORD_TNF_EXTERNAL 0x04
#define RECORD_TNF_UNKNOWN 0x05
#define RECORD_TNF_UNCHANGED 0x06

/**
 * Perform a full scan of an MiFare Type 2 device.
 *
 * @warning The MiFare tag must be in READY mode or this function will not work!
 */
esp_err_t ndef_full_scan(spi_device_handle_t* handle, tag_data_t* tag);

/**
 * Initializes an tag_data_t struct, most functions in the ndef module require
 * this data type to write data too.
 *
 * This data type stores all the NDEF records, however, you first need to
 * perform a `ndef_extract_all_records` to fill the array up with NDEF records.
 *
 * @warning This function creates heap memory, invoke `ndef_destroy_type`
 * inorder to properly destroy this struct
 *
 * @returns Creates an tag type with no data on it
 */
tag_data_t ndef_create_type();

/**
 * Destroys an tag_data_t struct. De-allocates any heap allocated memory and
 * empties the struct.
 *
 * @warning Calling this function twice on the same tag will cause a panic!
 */
void ndef_destroy_type(tag_data_t* tag);

/**
 * Moves the pointer of the tag to the nearest TLV payload byte. This function
 * will skip the starting bytes (in case the pointer hasn't been incremented
 * before), afterwards it will only iterate over TLV_NULL bytes.
 *
 * @deprecated Although this function still works, it is no longer in use by the
 * rest of the system and it's use is discouraged. For data parsing, use the
 * picc module instead
 *
 * @returns The length of the TLV payload
 */
uint8_t ndef_move_to_nearest_tlv(tag_data_t* tag);

/**
 * @returns Is the pointer pointing at an TLV_TERMINATE byte
 *
 * @deprecated Although this function still works, it is no longer in use by the
 * rest of the system and it's use is discouraged. For data parsing, use the
 * picc module instead
 */
bool ndef_is_end_of_tlv(tag_data_t* tag);

/**
 * Attempts to parse an NDEF record at the location of the current pointer.
 *
 * Trying to parse an record when the pointer isn't at a valid NDEF record
 * location will lead to unknown results. This does not mean that there is no
 * data validation done, but it is not fool-proof. The system is heavily reliant
 * on valid data being written to the tag.
 *
 * @warning The system is quite specific, it enforces the following set of
 * rules, failing to enforce them will return an error code.
 *  - NDEF records must be encoded with ASCII-US encoding. No UTF-8 support.
 *  - NDEF records must fit within a single record package, this the maximum
 * payload size is 255 bytes.
 *  - NDEF records must be the Well-Known mime type, specifically Well-Known,
 * Text record. Other record types are not supported.
 *
 * @warning An tag_data_t struct can hold at most `MAX_RECORD_COUNT` NDEF
 * records, attempting to parse more may result into invalid memory writes
 *
 * @deprecated Although this function still works, it is no longer in use by the
 * rest of the system and it's use is discouraged. For data parsing, use the
 * picc module instead
 *
 * @returns Possible error during the parsing
 */
esp_err_t ndef_parse_record(tag_data_t* tag);

/**
 * This function performs a full read cycle and extracts all NDEF records on an
 * tag. This function first performs an `ndef_full_scan`, then repeatidly
 * invokes `ndef_parse_record` until an TLV_TERMINATE is reached. If a MiFare
 * tag is properly formatted, this function would have extracted all NDEF
 * records from a tag without issue. This function has could have several
 * failure points, please refer to the documentation below to see the precise
 * points of failure.
 *
 * @see ndef_full_scan
 * @see ndef_parse_record
 * @see ndef_is_end_of_tlv
 *
 * @deprecated Although this function still works, it is no longer in use by the
 * rest of the system and it's use is discouraged. For data parsing, use the
 * picc module instead
 *
 * @param handle The SPI device handle
 * @param tag An empty NDEF data tag
 */
esp_err_t ndef_extract_all_records(
	spi_device_handle_t* handle, tag_data_t* tag
);

#endif
