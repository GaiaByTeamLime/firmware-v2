#include "sensors.h"
#include "adc.h"

esp_err_t capacity_sensor_init() {
	PASS_ERROR(
		gpio_set_direction(CAPACITY_SENSOR_PIN, GPIO_MODE_INPUT),
		"Could not set the CAPACITY_SENSOR_PIN to input."
	);
	PASS_ERROR(
		gpio_pulldown_dis(CAPACITY_SENSOR_PIN),
		"Could not disable pulldown on CAPACITY_SENSOR_PIN"
	);

	return ESP_OK;
}

esp_err_t ldr_sensor_init() {
	PASS_ERROR(
		gpio_set_direction(LDR_MEASUREMENT_PIN, GPIO_MODE_INPUT),
		"Could not set LDR_MEASUREMENT_PIN to input."
	);
	PASS_ERROR(
		gpio_pulldown_dis(LDR_MEASUREMENT_PIN),
		"Could not disable pulldown on LDR_MEASUREMENT_PIN"
	)
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
	PASS_ERROR(capacity_sensor_init(), "Could not init capacity sensor");
	PASS_ERROR(battery_measurement_init(), "Could not init battery measurement sensor.");

	return ESP_OK;
}

esp_err_t measure_sensors(uint32_t* data) {
	pull_latest_data(); // Run through ADC conversion system.

	get_adc_data(ADC1_CAP, &data[0]); // ADC1 Soil capacitor input.
	get_adc_data(ADC1_LDR, &data[1]); // ADC1 LDR sensor input.
	get_adc_data(ADC1_BAT, &data[2]); // ADC1 Battery sensor input.

	return ESP_OK;
}
