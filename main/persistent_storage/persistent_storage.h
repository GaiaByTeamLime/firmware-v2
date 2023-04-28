#ifndef __PERSISTENT_STORAGE_H__
#define __PERSISTENT_STORAGE_H__

#include <esp_err.h>
#include <nvs_flash.h>

esp_err_t persistent_storage_init();
esp_err_t persistent_storage_write();

#endif
