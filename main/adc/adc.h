#ifndef ADC_H_
#define ADC_H_

#include <esp_err.h>

#define ADC1_PORTS 1

struct ADC_data
{
	esp_err_t messageResult[ADC1_PORTS];
	int data[ADC1_PORTS]; //ldr //can be uint16_t, but create warnings
};

esp_err_t adc_init(void);

struct ADC_data getData(void);

#endif
