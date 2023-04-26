
#ifndef __PRELUDE_HEADER__
#define __PRELUDE_HEADER__

#include "esp_log.h"

#ifdef DEBUG
#define LOG(...) \
	ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO, __func__, __VA_ARGS__)
#else
#define LOG(...) /* */
#endif

#endif

