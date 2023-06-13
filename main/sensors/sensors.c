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
	);
	return ESP_OK;
}

esp_err_t battery_measurement_init() {
	PASS_ERROR(
		gpio_set_direction(BATTERY_MEASUREMENT_PIN, GPIO_MODE_INPUT),
		"Could not set BATTERY_MEASUREMENT_PIN to input."
	);
	PASS_ERROR(
		gpio_pulldown_dis(BATTERY_MEASUREMENT_PIN),
		"Could not disable BATTERY_MEASUREMENT_PIN pulldown resistor."
	);
	return ESP_OK;
}

esp_err_t sensors_init() {
	PASS_ERROR(capacity_sensor_init(), "Could not init capacity sensor");
	PASS_ERROR(
		battery_measurement_init(), "Could not init battery measurement sensor."
	);
	PASS_ERROR(ldr_sensor_init(), "Could not init ldr sensor.");

	return ESP_OK;
}

esp_err_t measure_sensors(uint32_t* data) {
	pull_latest_data(); // Run through ADC conversion system.

	PASS_ERROR(
		get_adc_data(ADC1_CAP, &data[0]), "Could not get ADC1_CAP data."
	); // ADC1 Soil capacitor input.
	PASS_ERROR(
		get_adc_data(ADC1_LDR, &data[1]), "Could not get ADC1_LDR data."
	); // ADC1 LDR sensor input.
	PASS_ERROR(
		get_adc_data(ADC1_BAT, &data[2]), "Could not get ADC1_BAT data."
	); // ADC1 Battery sensor input.

	return ESP_OK;
}
