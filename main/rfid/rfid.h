#ifndef __RFID_HEADER__
#define __RFID_HEADER__

#include "../prelude.h"

#include <esp_err.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#define RFID_SPI_HOST SPI2_HOST
#define RFID_SPI_MODE 0
#define RFID_SPI_CS_PIN 10
#define RFID_SPI_CLK_SPD 4000000

esp_err_t rfid_init(spi_device_handle_t* handle);

#endif
