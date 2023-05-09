#include "prelude.h"

#include "adc.h"

#include <esp_adc/adc_cali.h>
#include <esp_adc/adc_cali_scheme.h>
#include <esp_adc/adc_oneshot.h>

#define ADC1_UNIT ADC_UNIT_1
#define ADC1_ULP_MODE ADC_ULP_MODE_DISABLE
#define ADC1_ATTEN ADC_ATTEN_DB_11
#define ADC1_BITWIDTH ADC_BITWIDTH_DEFAULT

adc_oneshot_unit_handle_t adc1_handle;

adc_data_t adc_result[ADC1_PORTS] = {0};

/**
 * Initialize ADC1
 *
 * @return esp_err_t
 */
esp_err_t adc_init(void) {
	// ADC1 Init
	adc_oneshot_unit_init_cfg_t init_config1 = {
		.unit_id = ADC1_UNIT,
		.ulp_mode = ADC1_ULP_MODE, // sets if the ADC will be working under
								   // super low power mode (now disabled)
	};

	PASS_ERROR(
		adc_oneshot_new_unit(&init_config1, &adc1_handle), "ADC1 init failed"
	);

	// ADC1 Config
	adc_oneshot_chan_cfg_t config = {
		.bitwidth = ADC1_BITWIDTH,
		.atten = ADC1_ATTEN, // independent from input voltage. how higher the
							 // atten, how higher the input voltage can be
	};

	for (uint8_t i = 0; i < ADC1_PORTS; i++) {
		PASS_ERROR(
			adc_oneshot_config_channel(adc1_handle, i, &config),
			"ADC1 channel config failed"
		);
	}

	// ADC1 Calibration Init
	adc_cali_handle_t adc1_cali_handle = NULL;
	adc_cali_curve_fitting_config_t cali_config = {
		.unit_id = ADC1_UNIT,
		.atten = ADC1_ATTEN,
		.bitwidth = ADC1_BITWIDTH,
	};
	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);

	return ESP_OK;
}

/**
 * Get the Data from the ADC
 * data is stored in adc_data
 */
void pull_latest_data(void) {
	for (uint8_t i = 0; i < ADC1_PORTS; i++) {
		adc_result[i].messageResult = adc_oneshot_read(
			adc1_handle, i,
			(int*)&adc_result[i].data
		); // recommendend, doesn't work in an ISR context
		   // (instead, use the function adc_oneshot_read_isr())
	}
}

/**
 * Get the adc data object
 *
 * @param port (which sensordata to get)
 * @param data (pointer to the data)
 * @return esp_err_t
 */
esp_err_t get_adc_data(adc1_port_t port, uint32_t* data) {
	PASS_ERROR(adc_result[port].messageResult, "ADC1 Get Data Error");
	*data = adc_result[port].data;
	return ESP_OK;
}
