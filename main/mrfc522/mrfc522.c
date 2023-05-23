
#include "mrfc522.h"
#include "prelude.h"
#include "rfid.h"
#include "rfid_pcd_register_types.h"
#include "spi.h"

esp_err_t mrfc522_transceive_picc(
	spi_device_handle_t* handle, uint8_t* write_data,
	const uint16_t write_length, uint8_t* read_data, const uint16_t read_length,
	const uint8_t framing_bits
) {
	PASS_ERROR(
		rfid_send_command(handle, PCD_IDLE), "Unable to stop active command"
	);
	PASS_ERROR(
		rfid_write_register(handle, COM_IRQ_REG, CLEAR_IRQS),
		"Unable to clear all IRq's"
	);
	PASS_ERROR(
		rfid_write_register(handle, FIFO_LEVEL_REG, FLUSH_FIFO),
		"Unable to flush FIFO"
	);
	PASS_ERROR(
		rfid_write_register_datastream(
			handle, FIFO_DATA_REG, write_data, write_length
		),
		"Unable to stream data to FIFO buffer"
	);
	PASS_ERROR(
		rfid_write_register(handle, BIT_FRAMING_REG, framing_bits),
		"Unable to set bit adjustment"
	);
	PASS_ERROR(
		rfid_send_command(handle, PCD_TRANSCEIVE),
		"Unable to signal start of transmission"
	);
	PASS_ERROR(
		rfid_write_register(handle, BIT_FRAMING_REG, 0x80 | framing_bits),
		"Unable to signal start of transmission"
	)

	rfid_pcd_register_t registers[] = {COM_IRQ_REG, ERROR_REG, FIFO_LEVEL_REG};

	uint16_t attempts_left = 5000;
	uint8_t output_buffer[1] = {0};
	while (attempts_left) {
		PASS_ERROR(
			rfid_read_registers(handle, registers, output_buffer, 1),
			"Unable to read register"
		);
		// If the IRq fired, don't wait needlessly, break out
		if (output_buffer[0] & DATASTREAM_STOP_IRQ) {
			break;
		}
		if (output_buffer[0] & TIMEOUT_IRQ) {
			LOG("Timer Timeout");
			return ESP_ERR_TIMEOUT;
		}
		attempts_left--;
	}
	if (!attempts_left) {
		ELOG("Transceive took to long! Timed-out.");
		return ESP_ERR_TIMEOUT;
	}

	// Make sure no weird error happened in the meantime
	rfid_read_registers(handle, registers + 1, output_buffer, 1);
	if (output_buffer[0] & INVALID_STATE) {
		ELOG("Error during transceive!");
		return ESP_ERR_INVALID_STATE;
	}

	// Rea back the result!
	rfid_read_registers(handle, registers + 2, output_buffer, 1);
	uint8_t bytes_in_fifo = output_buffer[0];
	// LOG("Bytes in FIFO: %d", bytes_in_fifo);
	rfid_read_register_datastream(
		handle, FIFO_DATA_REG, read_data, bytes_in_fifo
	);

	return ESP_OK;
}

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
		rfid_write_register(handle, DIV_IRQ_REG, CLEAR_CRC_IRQ), "Unable to clear CRCIRq"
	);
	PASS_ERROR(
		rfid_write_register(handle, FIFO_LEVEL_REG, FLUSH_FIFO),
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
	uint16_t attempts_left = 5000;
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

esp_err_t mrfc522_read_mifare(
	spi_device_handle_t* handle, uint8_t block_address, uint8_t* buffer,
	uint16_t buffer_size
) {
	// First byte is the command (1 byte)
	// Then the arguments (block_address (1 byte) & crc (2 bytes))
	// The following 16 bytes will be filled with the returned data
	// The last 4 bits, a NAC is returned
	buffer[0] = MIFARE_READ;
	buffer[1] = block_address;
	// Calculate the CRC over the command & address
	PASS_ERROR(
		mrfc522_calculate_crc(handle, buffer, 2, buffer + 2),
		"Unable to calculate CRC"
	);
	// Send the transceive request
	const uint8_t framing = 0x00;
	return mrfc522_transceive_picc(
		handle, buffer, 4, buffer, buffer_size, framing
	);
}

esp_err_t mrfc522_wakeup(spi_device_handle_t* handle) {
	uint8_t command[1] = {MIFARE_REQA};
	uint8_t buffer[2];
	const uint8_t valid_bits = 7;

	// Make sure the device is in READY mode
	while (mrfc522_transceive_picc(handle, command, 1, buffer, 2, valid_bits) != ESP_OK) {
		LOG("Unable to wake up reader, trying again");
	}

	// Make a read at address 0x00, this will make the MiFare tag skip the
	// anti-collision logic and allow us to read straight from the tag.
	uint8_t empty_buff[18] = {0};
	PASS_ERROR(mrfc522_read_mifare(handle, 0, empty_buff, 18), "Unable to wake-up device");

	return ESP_OK;
}

esp_err_t mrfc522_enable_antenna(spi_device_handle_t* handle) {
	const uint16_t length = 1;
	rfid_pcd_register_t target_register[length];
	uint8_t response[length];
	target_register[0] = TX_CONTROL_REG;
	PASS_ERROR(
		rfid_read_registers(handle, target_register, response, length),
		"Unable to read from TxControlReg"
	);
	if (!(response[0] & ANTENNA_CONTROL_MASK)) {
		PASS_ERROR(
			rfid_write_register(
				handle, TX_CONTROL_REG, response[0] | ANTENNA_CONTROL_MASK
			),
			"Unable to write to TxControlReg"
		);
	}
	return ESP_OK;
}

esp_err_t mrfc522_disable_antenna(spi_device_handle_t* handle) {
	const uint16_t length = 1;
	rfid_pcd_register_t target_register[length];
	uint8_t response[length];
	target_register[0] = TX_CONTROL_REG;
	PASS_ERROR(
		rfid_read_registers(handle, target_register, response, length),
		"Unable to read from TxControlReg"
	);
	if (response[0] & ANTENNA_CONTROL_MASK) {
		PASS_ERROR(
			rfid_write_register(
				handle, TX_CONTROL_REG, response[0] & (~ANTENNA_CONTROL_MASK)
			),
			"Unable to write to TxControlReg"
		);
	}
	return ESP_OK;
}

esp_err_t mrfc522_init(spi_device_handle_t* handle) {
	// Reset registers
	PASS_ERROR(
		rfid_write_register(handle, TX_MODE_REG, 0x00), "Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, RX_MODE_REG, 0x00), "Unable to init MRFC522"
	);

	// Configure timer
	PASS_ERROR(
		rfid_write_register(handle, T_MODE_REG, TIMER_MODE), "Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, T_PRESCALER_REG, TIMER_PRESCALAR),
		"Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, T_RELOAD_REG_H, TIMER_DURATION >> 8),
		"Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, T_RELOAD_REG_L, (uint8_t)TIMER_DURATION),
		"Unable to init MRFC522"
	);

	PASS_ERROR(
		rfid_write_register(handle, TX_ASK_REG, FORCE_100_MODULATION), "Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, MODE_REG, CRC_PRESET_MODE), "Unable to init MRFC522"
	);

	// Enable the antenna
	mrfc522_enable_antenna(handle);

	// Wake up the rfid reader
	mrfc522_wakeup(handle);

	// Read actual data from the tag
	uint8_t buffer[18] = {0};
	mrfc522_read_mifare(handle, 4, buffer, 18);

	// Output the data
	LOG("SUCCES:");
	for (uint8_t i = 0; i < 18; i++) {
		LOG("\t%x (%c)", buffer[i], buffer[i]);
	}
	LOG("END");

	// Read actual data from the tag
	mrfc522_read_mifare(handle, 4 + 4, buffer, 18);

	// Output the data
	LOG("SUCCES:");
	for (uint8_t i = 0; i < 18; i++) {
		LOG("\t%x (%c)", buffer[i], buffer[i]);
	}
	LOG("END");

	return ESP_OK;
}
