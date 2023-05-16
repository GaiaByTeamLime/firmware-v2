

#include "mrfc522.h"
#include "rfid.h"
#include "rfid_pcd_register_types.h"

// Specific init codes found from:
// https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp

esp_err_t mrfc522_fifo_transaction(spi_device_handle_t* handle) {
	return ESP_OK;
}

esp_err_t mrfc522_init(spi_device_handle_t* handle) {
	// Reset registers
	PASS_ERROR(rfid_write_register(handle, TX_MODE_REG, 0x00), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, RX_MODE_REG, 0x00), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, MOD_WIDTH_REG, 0x00), "Unable to init MRFC522");

	// Configure timer
	PASS_ERROR(rfid_write_register(handle, T_MODE_REG, 0x80), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, T_PRESCALER_REG, 0xa9), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, T_RELOAD_REG_H, 0x03), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, T_RELOAD_REG_L, 0xe8), "Unable to init MRFC522");

	PASS_ERROR(rfid_write_register(handle, TX_ASK_REG, 0x40), "Unable to init MRFC522");
	PASS_ERROR(rfid_write_register(handle, MODE_REG, 0x3d), "Unable to init MRFC522");

	
	return ESP_OK;
}

