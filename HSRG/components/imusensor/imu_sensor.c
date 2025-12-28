#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "imu_sensor.h"
#include "freertos/queue.h"
#include "driver/i2c.h"

static const char* TAG = "IMU sensor";

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
} imu_data_t; 

/** 
 * @brief IMU sensor의 값을 받아오고 main의 Taskqueue로 보내주는 함수
 * @param[in] None
 * @retval IMU sensor value
*/
void imu_sensor_get_value(void* pvParameters) {
    imu_data_t imu_data;
    static esp_err_t err;
    QueueHandle_t imuQueue = (QueueHandle_t)pvParameters;

    while (1) {
        err = imu_init();
        if(err != ESP_OK) {
            ESP_LOGE(TAG, "failed to init imu sensor");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        ESP_LOGI(TAG, "succeed to init imu sensor");
    
        while(1) {
            float ax, ay, az, gx, gy, gz;

            err = imu_get(&ax, &ay, &az, &gx, &gy, &gz);
            if(err != ESP_OK) {
                ESP_LOGE(TAG, "failed to get imu sensor value");
                break; ;
            }

            imu_data.ax = ax; // 가속도
            imu_data.ay = ay;
            imu_data.az = az;
            imu_data.gx = gx; // 자이로
            imu_data.gy = gy;
            imu_data.gz = gz;

            ESP_LOGI(TAG, "succeed to get imu sensor value");
            if ((xQueueSend(imuQueue, &imu_data, 0)) != pdPASS) {
                ESP_LOGE(TAG, "failed to send imu data");
            }
            
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
}