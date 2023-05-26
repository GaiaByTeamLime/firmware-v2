#ifndef __ADC_HEADER__
#define __ADC_HEADER__

#include <esp_err.h>

/**
 * Number of ADC ports
 *
 */
#define ADC1_PORTS_AMOUNT 1

/**
 * Enum to select the adc port
 *
 */
typedef enum {
	ADC1_LDR = 0,
} adc1_port_t;

/**
 * Struct to store the adc data
 *
 */
typedef struct {
	esp_err_t messageResult;
	uint32_t data;
} adc_data_t;

/**
 * enum to select the adc port
 * 
 * @warning if you add some items, you need to change the ADC1_PORTS_AMOUNT and adc1_ports arrays in adc.c
 * 
 */
typedef enum {
	ADC1_LDR, //Port 3
} adc1_port_t;

/**
 * Initialize ADC1
 *
 * @return esp_err_t
 */
esp_err_t adc_init(void);

/**
 * Get the Data from the ADC
 * data is stored in adc_data
 */
void pull_latest_data(void);

/**
 * Get the adc data object
 *
 * @param port (which sensordata to get)
 * @param data (pointer to the data)
 * @return esp_err_t
 */
esp_err_t get_adc_data(adc1_port_t port, uint32_t* data);

#endif //__ADC_HEADER__
