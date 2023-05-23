#ifndef __MRFC522_H__
#define __MRFC522_H__

#include "rfid.h"

// Timer configuration
#define TIMER_DURATION 0x03e8
#define TIMER_PRESCALAR 0xa9
#define TIMER_MODE 0x80

// Other configuration
#define FORCE_100_MODULATION 0x40
#define CRC_PRESET_MODE 0x3d

// Some bit masks used to check for certrain interrupts etc
#define FLUSH_FIFO 0x80
#define ANTENNA_CONTROL_MASK 0x03
#define INVALID_STATE 0x13

// IRQs
#define CLEAR_IRQS 0x7f
#define CLEAR_CRC_IRQ 0x04
#define DATASTREAM_STOP_IRQ 0x30
#define TIMEOUT_IRQ 0x01

// The only two commands we need
#define MIFARE_READ 0x30
#define MIFARE_REQA 0x26

esp_err_t mrfc522_init(spi_device_handle_t* handle);

#endif
