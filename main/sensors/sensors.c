#include "sensors.h"
#include "adc.h"

static uint64_t count = 0;
static uint64_t prev_count = 0;
static bool timer_started = false;
static gptimer_handle_t timer_handle = NULL;
static uint32_t ldr_data;
static uint32_t prev_ldr_data;
static uint32_t bat_data;

static void interrupt_handler(void* args) {
	if (!timer_started) {
		gptimer_set_raw_count(timer_handle, 0);
		gptimer_start(timer_handle);
	} else {
		gptimer_stop(timer_handle);
		gptimer_get_raw_count(timer_handle, &count);
	}
	timer_started = !timer_started;
}

esp_err_t capacity_sensor_init() {
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

esp_err_t measure_battery_voltage() {
	PASS_ERROR(
		get_adc_data(ADC1_BAT, &bat_data),
		"Something went wrong on getting the battery voltage level."
	);
	LOG("Battery Voltage Measurement: %" PRIu32, bat_data);
	return ESP_OK;
}

esp_err_t sensors_init() {
	gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
	PASS_ERROR(capacity_sensor_init(), "Could not init capacity sensor");
	PASS_ERROR(battery_measurement_init(), "Could not init battery measurement sensor.");
  
	return ESP_OK;
}

esp_err_t measure_soil_capacity() {
	if (count != prev_count) {
		LOG("%d", (int)count);
	}
	prev_count = count;

	return ESP_OK;
}

esp_err_t measure_ldr() {
	get_adc_data(ADC1_LDR, &ldr_data);
	if (prev_ldr_data != ldr_data) {
		LOG("LDR Sensor measurement:  %" PRIu32, ldr_data);
	}

	return ESP_OK;
}
