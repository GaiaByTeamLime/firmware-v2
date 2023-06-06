#ifndef __SENSORS_HEADER__
#define __SENSORS_HEADER__

#include "../prelude.h"

#include <driver/gpio.h>
#include <driver/gptimer.h>
#include <esp_err.h>

#define CAPACITY_SENSOR_PIN 1
#define BATTERY_MEASUREMENT_PIN 4
#define CAPACITY_SENSOR_GPTIMER_RESOLUTION_HZ 40000000

/**
 * Initialize all sensors.
 * 
 * @return Potential errors.
*/
esp_err_t sensors_init();

/**
 * Set data[0] to newest soil capacity measurement value taken from pin IO1 if the new value is different than the old value.
 * This newest measurement will be taken by a interrupt.
 * 
 * @return Potential errors.
*/
esp_err_t measure_soil_capacity(uint32_t* data);

/**
 * Set measured LDR value into into the data array at position 1.
 * This measurement is taken beforehand by calling pull_latest_data(), and comes from pin IO3.
 * 
 * @return Potential errors.
*/
esp_err_t measure_ldr(uint32_t* data);

/**
 * Set measured Battery Voltage value into the data array at position 2.
 * This measurement is taken beforehand by calling pull_latest_data(), and comes from pin IO4.
 * 
 * @return Potential errors.
*/
esp_err_t measure_battery_voltage(uint32_t* data);

/**
 * Print out values from all 3 (0-2) positions of the data array.
 * 
 * @return Potential errors.
*/
esp_err_t print_measurements(uint32_t* data);

#endif
