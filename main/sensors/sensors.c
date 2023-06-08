#include "sensors.h"
#include "adc.h"

static uint64_t soil_capacity_data = 0;
static bool timer_started = false;
static gptimer_handle_t timer_handle = NULL;
static uint8_t soil_counter = 0;

static void interrupt_handler(void* args) {
	if (!timer_started) {
		gptimer_set_raw_count(timer_handle, 0);
		gptimer_start(timer_handle);
	} else {
		gptimer_stop(timer_handle);
		gptimer_get_raw_count(timer_handle, &soil_capacity_data);
		soil_counter++;
	}
	timer_started = !timer_started;
	
	// Only measure 5 times
	if (soil_counter >= 5) {
		gpio_isr_handler_remove(CAPACITY_SENSOR_PIN);
	}
}

esp_err_t capacity_sensor_init() {
	PASS_ERROR(
		gpio_set_direction(CAPACITY_SENSOR_POWER_PIN, GPIO_MODE_OUTPUT),
		"Could not set CAPACITY_SENSOR_POWER_PIN to mode output"
	);
	PASS_ERROR(
		gpio_set_level(CAPACITY_SENSOR_POWER_PIN, 1),
		"Could not set CAPACITY_SENSOR_PIN level to high. (1)"
	);
	PASS_ERROR(
		gpio_pullup_en(CAPACITY_SENSOR_POWER_PIN),
		"Could not enable pullup at CAPACITY_SENSOR_POWER_PIN"
	);

	PASS_ERROR(
		gpio_set_direction(CAPACITY_SENSOR_PIN, GPIO_MODE_INPUT),
		"Could not set CAPACITY_SENSOR_PIN to mode input"
	);
	PASS_ERROR(
		gpio_pulldown_dis(CAPACITY_SENSOR_PIN),
		"Could not disable pulldown resistor for CAPACITY_SENSOR_PIN"
	);
	PASS_ERROR(
		gpio_set_intr_type(CAPACITY_SENSOR_PIN, GPIO_INTR_POSEDGE),
		"Could not set interrupt type for CAPACITY_SENSOR_PIN"
	);
	PASS_ERROR(
		gpio_install_isr_service(0), "Could not install GPIO ISR service"
	);

	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ,
		.flags.intr_shared = 0,
	};

	PASS_ERROR(
		gpio_isr_handler_add(CAPACITY_SENSOR_PIN, interrupt_handler, NULL),
		"Could not add gpio isr handler for CAPACITY_SENSOR_PIN"
	);

	PASS_ERROR(
		gptimer_new_timer(&timer_config, &timer_handle),
		"Could not create new timer"
	);
	PASS_ERROR(gptimer_enable(timer_handle), "Could not enable timer");

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
	PASS_ERROR(
		battery_measurement_init(), "Could not init battery measurement sensor."
	);

	return ESP_OK;
}

esp_err_t measure_sensors(uint32_t* data) {
	pull_latest_data(); // Run through ADC conversion system.

	data[0] = soil_capacity_data;

	get_adc_data(ADC1_LDR, &data[1]); // ADC1 LDR sensor input.
	get_adc_data(ADC1_BAT, &data[2]); // ADC1 Battery sensor input.

	return ESP_OK;
}
