
#ifndef __PRELUDE_HEADER__
#define __PRELUDE_HEADER__

#include "esp_log.h"

// This is kind of reduntent, as the ESP already has log levels internally
// Though those are runtime checks, this is removed during compile time
// Which is preferable
// Still though
#ifdef DEBUG
#define LOG(...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, __func__, __VA_ARGS__)
#define ELOG(...) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR, __func__, __VA_ARGS__)
#else
#define LOG(...)			   /* */
#define ELOG(...)			   /* */
#endif

// TODO: make this only convert error code to message on debug mode
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
