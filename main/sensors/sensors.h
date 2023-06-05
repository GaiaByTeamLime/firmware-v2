#ifndef __SENSORS_HEADER__
#define __SENSORS_HEADER__

#include "../prelude.h"

#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <esp_err.h>

#define CAPACITY_SENSOR_PIN GPIO_NUM_1
#define BATTERY_MEASUREMENT_PIN GPIO_NUM_4
#define CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ 40000000

/**
 * Initialize all sensors.
 * 
 * @return Potential errors.
*/
esp_err_t sensors_init();

/**
 * Print out newest soil capacity measurement value taken from pin IO1 if the new value is different than the old value.
 * This newest measurement will be taken by a interrupt.
 * 
 * @return Potential errors.
*/
esp_err_t measure_soil_capacity();

/**
 * Set measured LDR value into ldr_data.
 * This measurement is taken beforehand by calling pull_latest_data(), and comes from pin IO3.
 * 
 * @return Potential errors.
*/
esp_err_t measure_ldr();

/**
 * Set measured Battery Voltage value into bat_data.
 * This measurement is taken beforehand by calling pull_latest_data(), and comes from pin IO4.
*/
esp_err_t measure_battery_voltage();

#endif
