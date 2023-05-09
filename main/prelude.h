#ifndef __PRELUDE_HEADER__
#define __PRELUDE_HEADER__

#include "esp_log.h"

// This is kind of reduntent, as the ESP already has log levels internally
// Though those are runtime checks, this is removed during compile time
// Which is preferable
// Still though
#ifdef DEBUG
/* Outputs text to the terminal, if DEBUG is NOT defined, this will be made into an empty comment, effectively removing it from the final build
 *
 * @param format The format for the message, uses the same format as printf
 * @param ... The arguments for the format
*/
#define LOG(...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, __func__, __VA_ARGS__)
/* Outputs text to the terminal, if DEBUG is NOT defined, this will be made into an empty comment, effectively removing it from the final build
 * This differs from LOG(...) by the fact it changes the severity to an error, rather than an info message
 * In compatible terminals, the text will be colored red
 *
 * @param format The format for the message, uses the same format as printf
 * @param ... The arguments for the format
*/
#define ELOG(...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, __func__, __VA_ARGS__)
#else
#define LOG(...)			   /* */
#define ELOG(...)			   /* */
#endif

/* Check if the code errors, if so, return from the parent function and pass the error along
 * This define should ONLY be used in functions whose return type is of esp_err_t, it WILL cause issues if this is not the case.
 * If DEBUG is defined, it will also output the error to the console
 *
 * TODO: Make this not convert to the name if DEBUG is defined
 *
 * @param code The function which needs to be checked
 * @param error_msg The error message to be added to the DEBUG log if the function returns an error
*/
#define PASS_ERROR(code, error_msg) \
	{ \
		esp_err_t err = (code); \
		if ((err) != ESP_OK) { \
			const char *err_name = esp_err_to_name(err); \
			ELOG("(%s) 0x%x: " error_msg, err_name, err); \
			return err; \
		} \
	}

#endif
