#include "main.h"

static const char* TAG = "Main";

void app_main(void) {
    ESP_LOGI(TAG, "1. init nvs flash");
    ESP_ERROR_CHECK(nvs_flash_init()); 
    vTaskDelay(pdMS_TO_TICKS(50));

    ESP_LOGI(TAG, "2. init wifi");
    wifi_init();
    vTaskDelay(pdMS_TO_TICKS(50));

    ESP_LOGI(TAG, "3. init esp information");
    espnow_deinit();
    espnow_init();
    vTaskDelay(pdMS_TO_TICKS(50));

    ESP_LOGI(TAG, "4. start main core task");
    xTaskCreate(main_core, "main_core", 4096, NULL, 5, NULL);
}   