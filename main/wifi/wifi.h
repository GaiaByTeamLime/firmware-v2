#ifndef __WIFI_HEADER__
#define __WIFI_HEADER__

#include <esp_err.h>

/**
 * Setup the entire WiFi driver and immediately connect to the network.
 * At the current moment, we only support WPA2-PSK encryption method
 *
 * @param esp_err_t(void) The callback function to be invoked once the device
 * connected to a network
 * @return Any error the WiFi driver may have encountered
 */
esp_err_t wifi_init(esp_err_t (*callback)(void));

/**
 * Start & connect to the WiFi network
 *
 * Connection to the network does NOT happen immediately, this function only
 * invokes the start of the network. The event listener then connects to the
 * network
 *
 * @param ssid The SSID of the WiFi network to connect to
 * @param password The password of the WiFi network to connect to
 * @return Any error the WiFi driver may have encountered
 */
esp_err_t wifi_start(const char* ssid, const char* password);

#endif
