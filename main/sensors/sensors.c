#include "sensors.h"
#include "adc.h"

esp_err_t capacity_sensor_init() {
	PASS_ERROR(
		gpio_set_direction(CAPACITY_SENSOR_PIN, GPIO_MODE_INPUT),
		"Could not set the capacity pin to input."
	);
	PASS_ERROR(
		gpio_pulldown_dis(CAPACITY_SENSOR_PIN),
		"Could not disable pulldown on capacity pin."
	)

	return ESP_OK;
}

esp_err_t battery_measurement_init() {
	PASS_ERROR(
		gpio_set_direction(BATTERY_MEASUREMENT_PIN, GPIO_MODE_INPUT),
		"Could not do the funny to Input"
	);
	PASS_ERROR(
		gpio_pulldown_dis(BATTERY_MEASUREMENT_PIN),
		"Could not disable internal pulldown resistor."
	);

	return ESP_OK;
}

esp_err_t sensors_init() {
	gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
	PASS_ERROR(capacity_sensor_init(), "Could not init capacity sensor");
	PASS_ERROR(battery_measurement_init(), "Could not init battery measurement sensor.");

	return ESP_OK;
}

esp_err_t measure_sensors(uint32_t* data) {
	pull_latest_data(); // Run through ADC conversion system.

	get_adc_data(ADC1_CAP, &data[0]); // ADC1 Soil capacitor input.
	get_adc_data(ADC1_LDR, &data[1]); // ADC1 LDR sensor input.
	get_adc_data(ADC1_BAT, &data[2]); // ADC1 Battery sensor input.

	gpio_uninstall_isr_service();
	return ESP_OK;
}
