
#ifndef __WIFI_HEADER__
#define __WIFI_HEADER__

#include <esp_err.h>

/* Setup the entire WiFi driver and immediately connect to the network
 * At the current moment, we only support WPA2-PSK encryption method
 *
 * @param ssid The SSID of the WiFi network to connect to
 * @param password The password of the WiFi network to connect to
 * @return esp_err_t Any error the WiFi driver may have encountered
*/
esp_err_t wifi_init(const char* ssid, const char* password);

#endif

