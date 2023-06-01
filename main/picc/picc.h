#ifndef _HEADER_PICC_
#define _HEADER_PICC_

#include "../prelude.h"

/**
 * The maximum length of the network SSID
 */
#define PICC_SSID_LENGTH 32
/**
 * The maximum length of the network password
 */
#define PICC_PASS_LENGTH 43
/**
 * The maximum length of the website URL
 */
#define PICC_SID_LENGTH SENSOR_ID_LENGTH
/**
 * The maximum length of the token sent to the server
 */
#define PICC_TOKEN_LENGTH SENSOR_TOKEN_LENGTH
/**
 * In case the token doesn't reach the max allowed size, then at what character
 * should it stop? This should be a null-byte '\0', but for testing purposes
 * I've made it `_`
 */
#define PICC_END_OF_STRING_CHAR '_'

#include <esp_err.h>

#include "ndef.h"

/**
 * Extract the SSID from the tag.
 *
 * @param tag The tag to extract the data from
 * @param ssid The string to store the ssid on, the length must be at least
 * `PICC_SSID_LENGTH + 1`
 */
void picc_get_ssid(tag_data_t* tag, char* ssid);
/**
 * Extract the password from the tag.
 *
 * @param tag The tag to extract the data from
 * @param password The string to store the password on, the length must be at
 * least `PICC_PASS_LENGTH + 1`
 */
void picc_get_password(tag_data_t* tag, char* password);
/**
 * Extract the url from the tag.
 *
 * @param tag The tag to extract the data from
 * @param url The string to store the url on, the length must be at least
 * `PICC_SID_LENGTH + 1`
 */
void picc_get_sid(tag_data_t* tag, char* url);
/**
 * Extract the token from an tag.
 *
 * @param tag The tag to extract the data from
 * @param token The string to store the token on, the length must be at least
 * `PICC_TOKEN_LENGTH + 1`
 */
void picc_get_token(tag_data_t* tag, char* token);

#endif
