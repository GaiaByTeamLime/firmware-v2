#include "sensors.h"

static uint32_t count = 0;

static void interrupt_handler(void* args) {
	gpio_set_level(GPIO_NUM_0, 1);
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
	PASS_ERROR(
		gpio_isr_handler_add(CAPACITY_SENSOR_PIN, interrupt_handler, NULL),
		"Could not add gpio isr handler for CAPACITY_SENSOR_PIN"
	);

	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ,
		.flags.intr_shared = 0,
	};
	gptimer_handle_t timer_handle = NULL;
	PASS_ERROR(gptimer_new_timer(&timer_config, &timer_handle), "Could not create new timer");
	PASS_ERROR(gptimer_enable(timer_handle), "Could not enable timer");

	return ESP_OK;
}

esp_err_t sensors_init() {
	gpio_set_direction(GPIO_NUM_0, GPIO_MODE_OUTPUT);
	PASS_ERROR(capacity_sensor_init(), "Could not init capacity sensor");

	return ESP_OK;
}
