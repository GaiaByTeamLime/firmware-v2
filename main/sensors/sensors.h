#ifndef __SENSORS_HEADER__
#define __SENSORS_HEADER__

#include "../prelude.h"

#include <esp_err.h>
#include <driver/gpio.h>
#include <driver/gptimer.h>

#define CAPACITY_SENSOR_PIN GPIO_NUM_1
#define CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ 40000000


esp_err_t sensors_init();
esp_err_t measure_soil_capacity();

#endif
