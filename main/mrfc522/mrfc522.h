#ifndef __MRFC522_H__
#define __MRFC522_H__

#include "rfid.h"

#define ANTENNA_CONTROL_MASK 0x03
esp_err_t mrfc522_init(spi_device_handle_t* handle);

#endif
