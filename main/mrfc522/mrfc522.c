
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "mrfc522.h"
#include "prelude.h"
#include "rfid.h"
#include "rfid_pcd_register_types.h"
#include "spi.h"

// Specific init codes found from:
// https://github.com/miguelbalboa/rfid/blob/master/src/MFRC522.cpp

esp_err_t mrfc522_fifo_transaction(spi_device_handle_t* handle) {
	PASS_ERROR(rfid_write_register(handle, COMMAND_REG, PCD_IDLE), "");
	PASS_ERROR(rfid_write_register(handle, COM_IRQ_REG, 0x7f), "");
	PASS_ERROR(rfid_write_register(handle, FIFO_LEVEL_REG, 0x80), "");

	const uint16_t length = 1;
	uint8_t data[length];
	PASS_ERROR(rfrid_write_register_datastream(handle, FIFO_DATA_REG, data, length), "");
esp_err_t mrfc522_calculate_crc(
	spi_device_handle_t* handle, uint8_t* data_to_calculate_crc,
	const uint16_t length, uint8_t* result
) {
	// Registers used by read_registers
	const rfid_pcd_register_t registers[3] = {
		DIV_IRQ_REG, CRC_RESULT_LSB_REG, CRC_RESULT_MSB_REG};

	// Tell the RFID device to calculate the CRC over our data
	PASS_ERROR(
		rfid_send_command(handle, PCD_IDLE), "Unable to stop active command"
	);
	PASS_ERROR(
		rfid_write_register(handle, DIV_IRQ_REG, 0x04), "Unable to clear CRCIRq"
	);
	PASS_ERROR(
		rfid_write_register(handle, FIFO_LEVEL_REG, 0x80),
		"Unable to flush FIFO buffer"
	);
	PASS_ERROR(
		rfid_write_register_datastream(
			handle, FIFO_DATA_REG, data_to_calculate_crc, length
		),
		"Unable to send data to FIFO buffer"
	);
	PASS_ERROR(
		rfid_send_command(handle, PCD_CALC_CRC), "Unable to calculate CRC"
	);

	// Wait for the IRQ signal to fire
	uint8_t attempts_left = 50; // Wait at max 50ms
	uint8_t irq_buffer[1] = {0};
	while (attempts_left) {
		PASS_ERROR(
			rfid_read_registers(handle, registers, irq_buffer, 1),
			"Unable to read register"
		);
		// If the IRq fired, don't wait needlessly, break out
		if (irq_buffer[0] & 0x04) {
			break;
		}
		// Wait before retrying
		vTaskDelay(pdMS_TO_TICKS(1));
		attempts_left--;
	}
	// Check if we didn't time out
	if (!attempts_left) {
		ELOG("CRC calculation took to long! Timed-out.");
		return ESP_ERR_TIMEOUT;
	}
	// Don't wrap your head around the following too much
	// Basically, we take the base pointer of the registers and add 1 or 2,
	// getting the 2nd element in that array We do the same for the result, we
	// take the base address, then add +0 or 1 and we get the destination
	// pointer
	rfid_read_registers(handle, registers + 1, result, 1);
	rfid_read_registers(handle, registers + 2, result + 1, 1);
	return ESP_OK;
}

esp_err_t mrfc522_enable_antenna(spi_device_handle_t* handle) {
	const uint16_t length = 1;
	rfid_pcd_register_t target_register[length];
	uint8_t response[length];
	target_register[0] = TX_CONTROL_REG;
	PASS_ERROR(rfid_read_registers(handle, target_register, response, length), "Unable to read from TxControlReg");
	if (!(response[0] & ANTENNA_CONTROL_MASK)) {
		PASS_ERROR(rfid_write_register(handle, TX_CONTROL_REG, response[0] | ANTENNA_CONTROL_MASK), "Unable to write to TxControlReg");
	}
	return ESP_OK;
}

esp_err_t mrfc522_disable_antenna(spi_device_handle_t* handle) {
	const uint16_t length = 1;
	rfid_pcd_register_t target_register[length];
	uint8_t response[length];
	target_register[0] = TX_CONTROL_REG;
	PASS_ERROR(rfid_read_registers(handle, target_register, response, length), "Unable to read from TxControlReg");
	if (response[0] & ANTENNA_CONTROL_MASK) {
		PASS_ERROR(rfid_write_register(handle, TX_CONTROL_REG, response[0] & (~ANTENNA_CONTROL_MASK)), "Unable to write to TxControlReg");
	}
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

	mrfc522_fifo_transaction(handle);

	return ESP_OK;
}

