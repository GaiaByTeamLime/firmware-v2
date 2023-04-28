#include "prelude.h"

#include "adc.h"

#include <esp_system.h>
#include <esp_adc/adc_oneshot.h>
//#include <esp_adc/adc_continuous.h>
#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <sdkconfig.h>

#define ADC1_UNIT ADC_UNIT_1
#define ADC1_ULP_MODE ADC_ULP_MODE_DISABLE
#define ADC1_ATTEN ADC_ATTEN_DB_11
#define ADC1_BITWIDTH ADC_BITWIDTH_DEFAULT
//#define PORT_LDR ADC_CHANNEL_2


adc_oneshot_unit_handle_t adc1_handle;

esp_err_t adc_init(void)
{
	esp_err_t message;

	//-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC1_UNIT,
		.ulp_mode = ADC1_ULP_MODE, //sets if the ADC will be working under super low power mode (now disabled)
    };
	
    message = adc_oneshot_new_unit(&init_config1, &adc1_handle);

	if (message != ESP_OK)
	{
		return message;
	}
	
    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC1_BITWIDTH,
        .atten = ADC1_ATTEN, //independent from input voltage. how higher the atten, how higher the input voltage can be
    };

	for (uint8_t i = 0; i < ADC1_PORTS; i++)
	{
		message = adc_oneshot_config_channel(adc1_handle, i, &config);

		if (message != ESP_OK)
		{
			return message;
		}
	}

	//-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
	adc_cali_curve_fitting_config_t cali_config = {
		.unit_id = ADC1_UNIT,
		.atten = ADC1_ATTEN,
		.bitwidth = ADC1_BITWIDTH,
	};
	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);

	return ESP_OK;
}

struct ADC_data getData(void) { //aanpassen
	struct ADC_data data;
	for (uint8_t i = 0; i < ADC1_PORTS; i++)
	{
		data.messageResult = adc_oneshot_read(adc1_handle, i, &data.data[i]);//recommendend, doesn't work in an ISR context (instead, use the function adc_oneshot_read_isr())
	}
	return data;
}
