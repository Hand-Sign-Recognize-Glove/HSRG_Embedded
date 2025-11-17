#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "imu_sensor.h"
#include "freertos/queue.h"
#include "driver/i2c.h"

static const char* TAG = "IMU sensor";

/** 
 * @brief IMU sensor의 값을 받아오고 main의 Taskqueue로 보내주는 함수
 * @param[in] None
 * @retval IMU sensor value
*/
void imu_sensor_get_value(void* pvParameters) {
    static esp_err_t err;
    QueueHandle_t imuQueue = (QueueHandle_t)pvParameters;

    err = imu_init();
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "failed to init imu sensor");
        return;
    }
    ESP_LOGI(TAG, "succeed to init imu sensor");
 
    while(1) {
        float ax, ay, az, gx, gy, gz;

        err = imu_get(&ax, &ay, &az, &gx, &gy, &gz);
        if(err != ESP_OK) {
            ESP_LOGE(TAG, "failed to get imu sensor value");
            return;
        }
        ESP_LOGI(TAG, "succeed to get imu sensor value");

        ESP_LOGI(TAG, "가속도 센서: %.2f %.2f %.2f  자이로 센서: %.2f %.2f %.2f\n",ax, ay, az, gx, gy, gz);

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}