#ifndef __ADC_HEADER__
#define __ADC_HEADER__

#include <esp_err.h>

#define ADC1_PORTS 1

typedef struct {
	esp_err_t messageResult;
	uint32_t data;
} adc_data_t;

typedef enum {
	ADC1_LDR = 0,
} adc1_port_t;

esp_err_t adc_init(void);
void pull_latest_data(void);
esp_err_t get_adc_data(adc1_port_t port, uint32_t* data);

#endif //__ADC_HEADER__
