#ifndef __RFID_HEADER__
#define __RFID_HEADER__

#include "../prelude.h"

#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

#define RFID_SPI_HOST SPI2_HOST
#define RFID_SPI_MODE 0
#define RFID_SPI_CS_PIN 10
#define RFID_SPI_CLK_SPD (4*1000*1000)
#define RFID_SPI_QUEUE_SIZE 1

esp_err_t rfid_init(spi_device_handle_t* handle);

#endif
