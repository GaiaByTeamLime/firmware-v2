#include "rfid.h"
#include "driver/spi_master.h"

#include "prelude.h"
#include "rfid_registers.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "spi_common.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

spi_device_handle_t spi_handle;

esp_err_t rfid_init(spi_device_handle_t* handle) {
	const spi_device_interface_config_t device_config = {
		.mode = SPI_TRANS_MODE_DIO,
		.spics_io_num = RFID_SPI_CS_PIN,
		.clock_speed_hz = RFID_SPI_CLK_SPD,
		.queue_size = RFID_SPI_QUEUE_SIZE,
	};

	PASS_ERROR(
		spi_bus_add_device(RFID_SPI_HOST, &device_config, handle),
		"Unable to add RFID SPI device to SPI host"
	);

	spi_handle = handle;

	init_PCB();

	return ESP_OK;
}

/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
 * because the D/C line needs to be toggled in the middle.)
 * This routine queues these commands up as interrupt transactions so they get
 * sent faster (compared to calling spi_device_transmit several times), and at
 * the mean while the lines for next transactions can get calculated.
 */
esp_err_t SPI_Send_Data(const uint8_t registerName, const uint8_t command) {
    int x;
    //Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
    //function is finished because the SPI driver needs access to it even while we're already calculating the next line.
    static spi_transaction_t trans[2];

    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (x=0; x<2; x++) {
        memset(&trans[x], 0, sizeof(spi_transaction_t));
        if ((x&1)==0) {
            //Even transfers are commands
            trans[x].length=8;
            trans[x].user=(void*)0;
        } else {
            //Odd transfers are data
            trans[x].length=8*4;
            trans[x].user=(void*)1;
        }
        trans[x].flags=SPI_TRANS_USE_TXDATA;
    }
    trans[0].tx_data[0]=registerName;           //Column Address Set
    trans[1].tx_data[0]=command;              //Start Col High

    //Queue all transactions.
    for (x=0; x<2; x++) {
        PASS_ERROR(spi_device_queue_trans(spi_handle, &trans[x], portMAX_DELAY), "Sending data failed");
    }

    //When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
    //mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
    //finish because we may as well spend the time calculating the next line. When that is done, we can call
    //send_line_finish, which will wait for the transfers to be done and check their status.

	return ESP_OK;
}

esp_err_t SPI_Send_Data(const uint8_t registerName,  bool keep_cs_active) {
    spi_transaction_t t;
	//u_int16_t data = (registerName << 8) | command;
    memset(&t, 0, sizeof(t));       //Zero out the transaction
    t.length=8;                     //Command is 16 bits
    t.tx_buffer=&registerName;               //The data is the cmd itself
    t.user=(void*)0;                //D/C needs to be set to 0
    if (keep_cs_active) {
      t.flags = SPI_TRANS_CS_KEEP_ACTIVE;   //Keep CS active after data transfer
    }
    PASS_ERROR(spi_device_polling_transmit(spi_handle, &t), "Sending polling data failed");  //Transmit!
    return ESP_OK;
}

static void check_send_status(void)
{
    spi_transaction_t *rtrans;
    //Wait for all 2 transactions to be done and get back the results.
    for (int x=0; x<2; x++) {
        PASS_ERROR(spi_device_get_trans_result(spi_handle, &rtrans, portMAX_DELAY), "Status check failed");
        //We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
    }
}

esp_err_t SPI_Get_Data(const uint8_t registerName, uint8_t * data) {
	spi_transaction_t t;
	// When using SPI_TRANS_CS_KEEP_ACTIVE, bus must be locked/acquired
    spi_device_acquire_bus(spi_handle, portMAX_DELAY);

	SPI_Send_Data(registerName | 0x80, true);

    memset(&t, 0, sizeof(t));
    t.length= 8;
    t.flags = SPI_TRANS_USE_RXDATA;
    t.user = (void*)1;

    PASS_ERROR(spi_device_polling_transmit(spi_handle, &t), "Getting data failed");

	*data = t.rx_data;

    // Release bus
    spi_device_release_bus(spi_handle);

	return ESP_OK;
}

//softwar reset
void PCB_Reset(void) {
	uint8_t count = 0;
	uint8_t n;

	SPI_Send_Data(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.

	// The datasheet does not mention how long the SoftRest command takes to complete.
	// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg) 
	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74μs. Let us be generous: 50ms.
	do {
		// Wait for the PowerDown bit in CommandReg to be cleared (max 3x50ms)
		vTaskDelay(50 / portTICK_PERIOD_MS);
		SPI_Get_Data(CommandReg, &n);
	} while ((n & (1 << 4)) && (++count) < 3);
} // End PCD_Reset()

    
/**
 * Turns the antenna on by enabling pins TX1 and TX2.
 * After a reset these pins are disabled.
 */
void PCD_AntennaOn(void) {
	uint8_t value;
	SPI_Get_Data(TxControlReg, &value);
	if ((value & 0x03) != 0x03) {
		SPI_Send_Data(TxControlReg, value | 0x03);
	}
} // End PCD_AntennaOn()

void init_PCB(void) {
	//reset the PCB
	PCB_Reset();

	// Reset baud rates
	SPI_Send_Data(TxModeReg, 0x00);
	SPI_Send_Data(RxModeReg, 0x00);
	// Reset ModWidthReg
	SPI_Send_Data(ModWidthReg, 0x26);

	// When communicating with a PICC we need a timeout if something goes wrong.
	// f_timer = 13.56 MHz / (2*TPreScaler+1) where TPreScaler = [TPrescaler_Hi:TPrescaler_Lo].
	// TPrescaler_Hi are the four low bits in TModeReg. TPrescaler_Lo is TPrescalerReg.
	SPI_Send_Data(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	SPI_Send_Data(TPrescalerReg, 0xA9);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25μs.
	SPI_Send_Data(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	SPI_Send_Data(TReloadRegL, 0xE8);
	
	SPI_Send_Data(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	SPI_Send_Data(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	
	PCD_AntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

