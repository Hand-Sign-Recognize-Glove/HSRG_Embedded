#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "imu_sensor.h"
#include "freertos/queue.h"
#include "driver/adc.h"

static const char* TAG = "Main Task";

void app_main(void)
{
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
}   
