#include <stdlib.h>

#include "../prelude.h"
#include "../spi/spi.h"
#include "rfid.h"

/**
 * How long before the timer times out
 * Combined with `TIMER_PRESCALAR` this determines the timeout period
 *
 * This is currently set to 1000, at a 40kHz frequency, this means the timeout
 * period is 25ms
 */
#define TIMER_DURATION 0x03e8
/**
 * Timer frequency formula
 * Freq = 13.56MHz / (2 * TIMER_PRESCALAR + 1)
 *
 * Currently it is set at 40 kHz
 */
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
// There are more commands, but the wakeup command (REQA) and the READ command
// are the only two we need
#define MIFARE_READ 0x30
#define MIFARE_REQA 0x26

esp_err_t rfid_write_register(
	spi_device_handle_t* handle, rfid_pcd_register_t reg, uint8_t data
) {
	return spi_send_word(handle, reg << 8 | data);
}

esp_err_t rfid_send_command(
	spi_device_handle_t* handle, rfid_pcd_command_t command
) {
	return rfid_write_register(handle, COMMAND_REG, command);
}

esp_err_t rfid_write_register_datastream(
	spi_device_handle_t* handle,
	rfid_pcd_register_t reg,
	uint8_t* datastream,
	const uint16_t length
) {
	uint8_t stream[length + 1];
	stream[0] = reg;
	for (uint16_t index = 1; index < length + 1; index++) {
		stream[index] = datastream[index - 1];
	}
	spi_send_datastream(handle, stream, length + 1);
	return ESP_OK;
}

esp_err_t rfid_read_register_datastream(
	spi_device_handle_t* handle,
	rfid_pcd_register_t reg,
	uint8_t* output_stream,
	const uint16_t length
) {
	uint8_t write_stream[length + 1], shifted_buffer[length + 1];
	write_stream[length] = 0; // Add one extra to get the final byte
	for (uint16_t index = 0; index < length - 1; index++) {
		write_stream[index] = reg | 0x80; // Read the register (0x80)
	}

	spi_transaction_t transaction = {0};
	transaction.tx_buffer = write_stream;
	transaction.rx_buffer = shifted_buffer;
	transaction.length = (length + 1) * 8;

	PASS_ERROR(
		spi_device_transmit(*handle, &transaction), "Unable to transmit data"
	);

	// The output buffer is shifted by 1, this corrects that error
	for (uint8_t index = 1; index < length + 1; index++) {
		output_stream[index - 1] = shifted_buffer[index];
	}

	return ESP_OK;
}

esp_err_t rfid_read_registers(
	spi_device_handle_t* handle,
	const rfid_pcd_register_t* registers,
	uint8_t* buffer,
	const uint16_t length
) {
	rfid_pcd_register_t read_registers[length];

	// Make sure the registers we send are marked as 'read'
	for (uint16_t index = 0; index < length; index++) {
		read_registers[index] = registers[index] | 0x80;
	}

	// Create small buffers for each transaction
	uint8_t current_register[2] = {0}; // [0] = The register, [1] = Always 0
	uint8_t rx_buffer[2];			   // [0] = Ignore, [1] = The data
	for (uint16_t index = 0; index < length; index++) {
		current_register[0] = read_registers[index];

		spi_transaction_t transaction = {0};
		transaction.tx_buffer = current_register;
		transaction.length = 16;
		transaction.rx_buffer = rx_buffer;

		PASS_ERROR(
			spi_device_transmit(*handle, &transaction), "Unable to transmit"
		);

		buffer[index] = rx_buffer[1];
	}

	return ESP_OK;
}

esp_err_t rfid_transceive(
	spi_device_handle_t* handle,
	uint8_t* write_data,
	const uint16_t write_length,
	uint8_t* read_data,
	const uint16_t read_length,
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
	uint8_t interrupt_register = 0;
	while (attempts_left) {
		PASS_ERROR(
			rfid_read_registers(handle, registers, &interrupt_register, 1),
			"Unable to read register"
		);
		// If the IRq fired, don't wait needlessly, break out
		if (interrupt_register & DATASTREAM_STOP_IRQ) {
			break;
		}
		if (interrupt_register & TIMEOUT_IRQ) {
			ELOG("Timer Timeout");
			return ESP_ERR_TIMEOUT;
		}
		attempts_left--;
	}
	if (!attempts_left) {
		ELOG("Transceive took to long! Timed-out.");
		return ESP_ERR_TIMEOUT;
	}

	// Make sure no weird error happened in the meantime
	PASS_ERROR(
		rfid_read_registers(handle, registers + 1, &interrupt_register, 1),
		"Unable to read error register"
	);
	if (interrupt_register & INVALID_STATE) {
		ELOG("Error during transceive!");
		return ESP_ERR_INVALID_STATE;
	}

	// Read back the result!
	PASS_ERROR(
		rfid_read_registers(handle, registers + 2, &interrupt_register, 1),
		"Unable to read bytes in FIFO register"
	);
	uint8_t bytes_in_fifo = interrupt_register;
	PASS_ERROR(
		rfid_read_register_datastream(
			handle, FIFO_DATA_REG, read_data, bytes_in_fifo
		),
		"Unable to read FIFI data register"
	);

	return ESP_OK;
}

esp_err_t rfid_calculate_crc(
	spi_device_handle_t* handle,
	uint8_t* data_to_calculate_crc,
	const uint16_t length,
	uint8_t* result
) {
	// Registers used by read_registers
	const rfid_pcd_register_t registers[3] = {
		DIV_IRQ_REG, CRC_RESULT_LSB_REG, CRC_RESULT_MSB_REG};

	// Tell the RFID device to calculate the CRC over our data
	PASS_ERROR(
		rfid_send_command(handle, PCD_IDLE), "Unable to stop active command"
	);
	PASS_ERROR(
		rfid_write_register(handle, DIV_IRQ_REG, CLEAR_CRC_IRQ),
		"Unable to clear CRCIRq"
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
	PASS_ERROR(
		rfid_read_registers(handle, registers + 1, result, 2),
		"Unable to read RFID result"
	);
	return ESP_OK;
}

esp_err_t rfid_read_mifare_tag(
	spi_device_handle_t* handle,
	uint8_t block_address,
	uint8_t* buffer,
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
		rfid_calculate_crc(handle, buffer, 2, buffer + 2),
		"Unable to calculate CRC"
	);
	// Send the transceive request
	const uint8_t framing = 0x00;
	return rfid_transceive(handle, buffer, 4, buffer, buffer_size, framing);
}

esp_err_t rfid_wakeup_mifare_tag(spi_device_handle_t* handle) {
	uint8_t command[1] = {MIFARE_REQA};
	uint8_t buffer[2];
	const uint8_t valid_bits = 7;

	// Make sure the device is in READY mode
	while (rfid_transceive(handle, command, 1, buffer, 2, valid_bits) != ESP_OK
	) {
		LOG("Unable to wake up reader, trying again");
	}

	// Make a read at address 0x00, this will make the MiFare tag skip the
	// anti-collision logic and allow us to read straight from the tag.
	uint8_t empty_buff[16] = {0};
	PASS_ERROR(
		rfid_read_mifare_tag(handle, 0, empty_buff, 16),
		"Unable to wake-up device"
	);

	return ESP_OK;
}

esp_err_t rfid_enable_antenna(spi_device_handle_t* handle) {
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

esp_err_t rfid_disable_antenna(spi_device_handle_t* handle) {
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

esp_err_t rfid_init(spi_device_handle_t* handle) {
	const spi_device_interface_config_t device_config = {
		.mode = RFID_SPI_MODE,
		.spics_io_num = RFID_SPI_CS_PIN,
		.clock_speed_hz = RFID_SPI_CLK_SPD,
		.queue_size = RFID_SPI_QUEUE_SIZE,
	};

	PASS_ERROR(
		spi_bus_add_device(RFID_SPI_HOST, &device_config, handle),
		"Unable to add RFID SPI device to SPI host"
	);

	// Reset registers
	PASS_ERROR(
		rfid_write_register(handle, TX_MODE_REG, 0x00), "Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, RX_MODE_REG, 0x00), "Unable to init MRFC522"
	);

	// Configure timer
	PASS_ERROR(
		rfid_write_register(handle, T_MODE_REG, TIMER_MODE),
		"Unable to init MRFC522"
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
		rfid_write_register(handle, TX_ASK_REG, FORCE_100_MODULATION),
		"Unable to init MRFC522"
	);
	PASS_ERROR(
		rfid_write_register(handle, MODE_REG, CRC_PRESET_MODE),
		"Unable to init MRFC522"
	);

	// Enable the antenna
	return rfid_enable_antenna(handle);
}
