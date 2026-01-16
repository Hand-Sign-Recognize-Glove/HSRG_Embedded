#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "esp_now_set.h"
#include "ble.h"
#include "imu_sensor.h"

static const char* TAG = "Main Task";

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init()); 
    float flex_values[5] = { -1 };
    espnow_event_t evt;
    imuData imuSensorData;

    QueueHandle_t flexQueue = xQueueCreate(10, sizeof(uint16_t) * 5); 
    if (!flexQueue) {
        ESP_LOGE(TAG, "flex sensor failed to create new queue");
    }
 
    QueueHandle_t imuQueue = xQueueCreate(10, sizeof(imuSensorData));
    if (!imuQueue) {
        ESP_LOGE(TAG, "imu sensor failed to create new queue");
    }

    QueueHandle_t espnowDataQueue = xQueueCreate(10, sizeof(espnow_event_t));
    if (!espnowDataQueue) {
        ESP_LOGE(TAG, "espnow faile to create new queue");
    }

    xTaskCreate(flex_sensor_get_value, "flex_sensor_get_value", 4096, (void*)flexQueue, 5, NULL);
    xTaskCreate(imu_sensor_task, "imu_sensor_task", 4096, (void*)imuQueue, 5, NULL);
    xTaskCreate(espnow_task, "esp_now", 4096, (void*)espnowDataQueue, 5, NULL);
    xTaskCreate(ble_main_task, "ble_main_task", 4096, NULL, 5, NULL);

    wifi_init();
    espnow_deinit();
    espnow_init();

    while(1) {
        if (xQueueReceive(imuQueue, &imuSensorData, 0) == pdPASS) {
            ESP_LOGI(TAG, "ACC: %.2f %.2f %.2f  GYR: %.2f %.2f %.2f",
            imuSensorData.accel_g[0][0],
            imuSensorData.accel_g[0][1],
            imuSensorData.accel_g[0][2],
            imuSensorData.gyro_dps[0][0],
            imuSensorData.gyro_dps[0][1],
            imuSensorData.gyro_dps[0][2]);
        } 
        if (xQueueReceive(flexQueue, flex_values, 0) == pdPASS) {
            for(int i = 0; i < 5; i++) {
                ESP_LOGI(TAG, "finger%d : %.2f", i, flex_values[i]);
            }
        }
        if (xQueueReceive(espnowDataQueue, &evt, 0) == pdPASS) {   
            espnow_data_t *buf = (espnow_data_t *)evt.recv_cb.data;

            ESP_LOGI(TAG, "recv: type=%d seq=%d magic=%d", buf->type, buf->seq_num, buf->magic);
        } 
    }
}   