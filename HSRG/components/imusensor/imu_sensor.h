#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t imu_init(void);

esp_err_t imu_get(float *ax, float *ay, float *az, float *gx, float *gy, float *gz);

void imu_sensor_get_value(void* pvParameters);  

#ifdef __cplusplus
}
#endif
