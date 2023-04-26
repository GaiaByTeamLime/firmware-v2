#include "prelude.h"

#include "esp_system.h"
#include "esp_adc/adc_oneshot.h"
//#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "sdkconfig.h"

adc_oneshot_unit_handle_t adc1_handle;

void adc_init(void)
{
	//-------------ADC1 Init---------------//
    adc_oneshot_unit_init_cfg_t init_config1 = {
        .unit_id = ADC_UNIT_1,
		.ulp_mode = ADC_ULP_MODE_DISABLE, //sets if the ADC will be working under super low power mode (now disabled)
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config1, &adc1_handle));

    //-------------ADC1 Config---------------//
    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11, //afhankelijk van input voltage. hoe hoger de atten, hoe hoger de input voltage mag zijn
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_2, &config));

	//-------------ADC1 Calibration Init---------------//
    adc_cali_handle_t adc1_cali_handle = NULL;
	adc_cali_curve_fitting_config_t cali_config = {
		.unit_id = ADC_UNIT_1,
		.atten = ADC_ATTEN_DB_11,
		.bitwidth = ADC_BITWIDTH_DEFAULT,
	};
	adc_cali_create_scheme_curve_fitting(&cali_config, &adc1_cali_handle);
}

void app_main(void) {
	int index = 0;
	int adc_raw = 0;

	adc_init();

	while (1) {
		ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ADC_CHANNEL_2, &adc_raw)); //aangeraden manier, werkt niet in ISR context (daarvoor is de functie adc_oneshot_read_isr())
        LOG("ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHANNEL_2, adc_raw);
		LOG("Test! %d", index++);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}
