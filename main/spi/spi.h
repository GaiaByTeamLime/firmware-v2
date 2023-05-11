#ifndef __SPI_HEADER__
#define __SPI_HEADER__

#include "../prelude.h"

#include <driver/spi_common.h>
#include <driver/spi_master.h>
#include <esp_err.h>

#define SPI2_MOSI_PIN 7
#define SPI2_MISO_PIN 2
#define SPI2_SCLK_PIN 6
#define SPI2_QUADWP_PIN -1
#define SPI2_QUADHD_PIN -1
#define SPI2_MAX_TRANSFER_SIZE SOC_SPI_MAXIMUM_BUFFER_SIZE

/**
 * Initialize the SPI2 Host
 */
esp_err_t spi2_init();

#endif
