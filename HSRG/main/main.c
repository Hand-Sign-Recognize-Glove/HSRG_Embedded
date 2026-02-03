#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_now_set.h"
#include "ble.h"
#include "main_core.h"
// #include "imu_sensor.h"

static const char* TAG = "Main";

void app_main(void) {
    ESP_LOGI(TAG, "1. init nvs flash");
    ESP_ERROR_CHECK(nvs_flash_init()); 

    ESP_LOGI(TAG, "2. init wifi");
    wifi_init();

    ESP_LOGI(TAG, "3. init esp information");
    espnow_deinit();
    espnow_init();

    ESP_LOGI(TAG, "4. start main core task");
    xTaskCreate(main_core, "main_core", 4096, NULL, 5, NULL);
}   