#ifndef __SENSORS_HEADER__
#define __SENSORS_HEADER__

#include "../prelude.h"

#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <esp_err.h>

#define CAPACITY_SENSOR_PIN 1
#define BATTERY_MEASUREMENT_PIN 4
#define LDR_MEASUREMENT_PIN 3
#define CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ 40000000

/**
 * Initialize all sensors.
 *
 * @return Potential errors.
 */
esp_err_t sensors_init();

/**
 * Function to take measurements, and place the taken measurements into a array
 * that is given in the form of a pointer. During debugging mode, 3 log
 * statements will display the first 3 sensor measurements stored in the table
 * given.
 *
 * @param data A pointer to a uint32_t array that will contain the sensor data.
 *
 * @warning Make sure that the given array is the same size as amount of
 * sensors. At the time of writing this, this number is 3.
 *
 * @return Potential errors.
 */
esp_err_t measure_sensors(uint32_t* data);

#endif
