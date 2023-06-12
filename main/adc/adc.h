#ifndef __ADC_HEADER__
#define __ADC_HEADER__

#include <esp_err.h>

/**
 * The amount of ADC ports used
 *
 * @warning If you add or remove an item from the `adc1_port_t` enum, you need
 * to change this define
 *
 */
#define ADC1_PORTS 3

/**
 * Enum to select the corresponding adc port
 *
 * @warning If you add or remove an item from this enum, you need to change the
 * ADC1_PORTS with the amount of items in this enum. Also, you need to
 * add or remove the corresponding port from the `adc1_ports` array in adc.c.
 * The 0 index of the array is the 0 index of the enum
 *
 */
typedef enum {
	ADC1_LDR = 0, /*!< Is connected to PORT 3 from the esp32. */
	ADC1_BAT = 1, /*!< Is connected to PORT 4 from the esp32. */
	ADC1_CAP = 2, /*!< Is connected to PORT 1 from the esp32. */
} adc1_port_t;

/**
 * Struct to store the adc data
 *
 * @note The data is stored in the `adc_result` array in adc.c. The 0 index of
 * the array is the 0 index of the enum `adc_result`
 *
 * @param messageResult Result of the operation
 * @param data The data from the ADC
 *
 */
typedef struct {
	esp_err_t messageResult;
	uint32_t data;
} adc_data_t;

/**
 * Initialize ADC1
 *
 * @return Result of the initialization
 */
esp_err_t adc_init(void);

/**
 * Update the Data from the ADC
 *
 * @note Data is stored in adc_data
 */
void pull_latest_data(void);

/**
 * Get the adc data from the specified port
 *
 * @param port Which port to get the data from
 * @param data Pointer in wich the data will be stored
 * @return Result of the data collection
 */
esp_err_t get_adc_data(adc1_port_t port, uint32_t* data);

#endif //__ADC_HEADER__
