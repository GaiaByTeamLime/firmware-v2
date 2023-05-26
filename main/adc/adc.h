#ifndef __ADC_HEADER__
#define __ADC_HEADER__

#include <esp_err.h>

#define ADC1_PORTS_AMOUNT 1

typedef struct {
	esp_err_t messageResult;
	uint32_t data;
} adc_data_t;

typedef enum { //if you add some items, you need to change the ADC1_PORTS_AMOUNT and adc1_ports arrays in adc.c
	ADC1_LDR, //Port 3
} adc1_port_t;

esp_err_t adc_init(void);
void pull_latest_data(void);
esp_err_t get_adc_data(adc1_port_t port, uint32_t* data);

#endif //__ADC_HEADER__
