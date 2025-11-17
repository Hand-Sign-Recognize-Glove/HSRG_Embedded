#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "imu_sensor.h"
#include "freertos/queue.h"
#include "driver/adc.h"

static const char* TAG = "Main Task";

typedef struct {
    float ax;
    float ay;
    float az;
    float gx;
    float gy;
    float gz;
} imu_data_t;


void app_main(void) {
    imu_data_t imuData;

    QueueHandle_t flexQueue = xQueueCreate(10, sizeof(float)); 
    if (flexQueue != ESP_OK) {
        ESP_LOGE(TAG, "flex sensor failed to create new queue");
    }

    QueueHandle_t imuQueue = xQueueCreate(10, sizeof(float));
    if (imuQueue != ESP_OK) {
        ESP_LOGE(TAG, "imu sensor faile to create new queue");
    }

    xTaskCreate(flex_sensor_get_value, "flex_sensor_get_value", 4096, (void*)flexQueue, 5, NULL);
    xTaskCreate(imu_sensor_get_value, "imu_sensor_get_value", 4096, (void*)imuQueue, 5, NULL);

    while(1) {
        if(xQueueReceive(imuQueue, &imuData, portMAX_DELAY) == pdPASS) {
            ESP_LOGI(TAG, "ACC: %.2f %.2f %.2f  GYR: %.2f %.2f %.2f\n", imuData.ax, imuData.ay, imuData.az, imuData.gx, imuData.gy, imuData.gz);
        }
        else {
            ESP_LOGE(TAG, "failed to receive imuQueue data");
        }
    }
}   
