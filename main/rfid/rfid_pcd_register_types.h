typedef enum {
	// Page 0: Command and status
	//						  0x00			// reserved for future use
	COMMAND_REG = 0x01 << 1, // starts and stops command execution
	COM_IEN_REG =
		0x02 << 1, // enable and disable interrupt request control bits
	DIV_IEN_REG =
		0x03 << 1, // enable and disable interrupt request control bits
	COM_IRQ_REG = 0x04 << 1, // interrupt request bits
	DIV_IRQ_REG = 0x05 << 1, // interrupt request bits
	ERROR_REG = 0x06 << 1, // error bits showing the error status of the last //
						   // command executed
	STATUS1_REG = 0x07 << 1,	// communication status bits
	STATUS_2_REG = 0x08 << 1,	// receiver and transmitter status bits
	FIFO_DATA_REG = 0x09 << 1,	// input and output of 64 byte FIFO buffer
	FIFO_LEVEL_REG = 0x0A << 1, // number of bytes stored in the FIFO buffer
	WATER_LEVEL_REG =
		0x0B << 1,			 // level for FIFO underflow and overflow warning
	CONTROL_REG = 0x0C << 1, // miscellaneous control registers
	BIT_FRAMING_REG = 0x0D << 1, // adjustments for bit-oriented frames
	COLL_REG = 0x0E << 1, // bit position of the first bit-collision detected on
						  // the RF interface
	//						  0x0F			// reserved for future use

	// Page 1: Command
	// 						  0x10			// reserved for future use
	MODE_REG =
		0x11 << 1, // defines general modes for transmitting and receiving
	TX_MODE_REG = 0x12 << 1,	// defines transmission data rate and framing
	RX_MODE_REG = 0x13 << 1,	// defines reception data rate and framing
	TX_CONTROL_REG = 0x14 << 1, // controls the logical behavior of the antenna
								// driver pins TX1 and TX2
	TX_ASK_REG =
		0x15 << 1, // controls the setting of the transmission modulation
	TX_SEL_REG =
		0x16 << 1, // selects the internal sources for the antenna driver
	RX_SEL_REG = 0x17 << 1,		  // selects internal receiver settings
	RX_THRESHOLD_REG = 0x18 << 1, // selects thresholds for the bit decoder
	DEMOD_REG = 0x19 << 1,		  // defines demodulator settings
	// 						  0x1A			// reserved for future use
	// 						  0x1B			// reserved for future use
	MF_TX_REG =
		0x1C << 1, // controls some MIFARE communication transmit parameters
	MF_RX_REG =
		0x1D << 1, // controls some MIFARE communication receive parameters
	// 						  0x1E			// reserved for future use
	SERIAL_SPEED_REG = 0x1F
					   << 1, // selects the speed of the serial UART interface

	// Page 2: Configuration
	// 						  0x20			// reserved for future use
	CRC_RESULT_MSB_REG =
		0x21 << 1, // shows the MSB and LSB values of the CRC calculation
	CRC_RESULT_LSB_REG = 0x22 << 1,
	// 						  0x23			// reserved for future use
	MOD_WIDTH_REG = 0x24 << 1, // controls the ModWidth setting?
	// 						  0x25			// reserved for future use
	RF_CFG_REG = 0x26 << 1, // configures the receiver gain
	GS_N_REG = 0x27 << 1, // selects the conductance of the antenna driver pins
						  // TX1 and TX2 for modulation
	CW_GS_P_REG = 0x28 << 1,  // defines the conductance of the p-driver output
							  // during periods of no modulation
	MOD_GS_P_REG = 0x29 << 1, // defines the conductance of the p-driver output
							  // during periods of modulation
	T_MODE_REG = 0x2A << 1,	  // defines settings for the internal timer
	T_PRESCALER_REG = 0x2B << 1, // the lower 8 bits of the TPrescaler value.
								 // The 4 high bits are in TModeReg.
	T_RELOAD_REG_H = 0x2C << 1, // defines the 16-bit timer reload value
	T_RELOAD_REG_L = 0x2D << 1,
	T_COUNTER_VALUE_REG_H = 0x2E << 1, // shows the 16-bit timer value
	T_COUNTER_VALUE_REG_L = 0x2F << 1,

	// Page 3: Test Registers
	// 						  0x30			// reserved for future use
	TEST_SEL1_REG = 0x31 << 1,	 // general test signal configuration
	TEST_SEL2_REG = 0x32 << 1,	 // general test signal configuration
	TEST_PIN_EN_REG = 0x33 << 1, // enables pin output driver on pins D1 to D7
	TEST_PIN_VALUE_REG =
		0x34
		<< 1, // defines the values for D1 to D7 when it is used as an I/O bus
	TEST_BUS_REG = 0x35 << 1,	 // shows the status of the internal test bus
	AUTO_TEST_REG = 0x36 << 1,	 // controls the digital self-test
	VERSION_REG = 0x37 << 1,	 // shows the software version
	ANALOG_TEST_REG = 0x38 << 1, // controls the pins AUX1 and AUX2
	TEST_DAC1_REG = 0x39 << 1,	 // defines the test value for TestDAC1
	TEST_DAC2_REG = 0x3A << 1,	 // defines the test value for TestDAC2
	TEST_ADC_REG = 0x3B << 1	 // shows the value of ADC I and Q channels
	// 						  0x3C			// reserved for production tests
	// 						  0x3D			// reserved for production tests
	// 						  0x3E			// reserved for production tests
	// 						  0x3F			// reserved for production tests
} rfid_pcd_register_t;

// MFRC522 commands. Described in chapter 10 of the datasheet.
typedef enum {
	PCD_IDLE = 0x00, // no action, cancels current command execution
	PCD_MEM = 0x01,	 // stores 25 bytes into the internal buffer
	PCD_GENERATE_RANDOM_ID = 0x02, // generates a 10-byte random ID number
	PCD_CALC_CRC =
		0x03, // activates the CRC coprocessor or performs a self-test
	PCD_TRANSMIT = 0x04,	  // transmits data from the FIFO buffer
	PCD_NO_CMD_CHANGE = 0x07, // no command change, can be used to modify the
							  // CommandReg register bits without affecting the
							  // command, for example, the PowerDown bit
	PCD_RECEIVE = 0x08,		  // activates the receiver circuits
	PCD_TRANSCEIVE =
		0x0C, // transmits data from FIFO buffer to antenna and automatically
			  // activates the receiver after transmission
	PCD_MF_AUTHENT =
		0x0E, // performs the MIFARE standard authentication as a reader
	PCD_SOFT_RESET = 0x0F // resets the MFRC522
} rfid_pcd_command_t;
