#include "flex_sensor.h"

#define CH0 ADC1_CHANNEL_0
#define CH1 ADC1_CHANNEL_3
#define CH2 ADC1_CHANNEL_6
#define CH3 ADC1_CHANNEL_7
#define CH4 ADC1_CHANNEL_4

static const char* TAG = "flex Sensor";

void flex_sensor_get_value(void* pvParameters) {
    static esp_err_t err;
    uint16_t value_arr[5];
    QueueHandle_t flexQueue = (QueueHandle_t)pvParameters;

    if (flexQueue == NULL) {
        ESP_LOGE(TAG, "flexQueue is NULL");
        vTaskDelete(NULL);
    }
    
    static const adc1_channel_t flex_channels[5] = {CH0, CH1, CH2, CH3, CH4};

    err = adc1_config_width(ADC_WIDTH_BIT_12); // 몇 비트 읽을지 결정, 높을수록 섬세함
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail to config adc width / Error name : %s", esp_err_to_name(err));
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Succeed to config adc width");

    for(int i = 0; i < 5; i++) {
        err = adc1_config_channel_atten(flex_channels[i], ADC_ATTEN_DB_11); // 감쇠 정도 (flex sensor에 11decibel까지 줄여서 전달)
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to reset GPIO channel : %d / Error name : %s", i, esp_err_to_name(err));
            vTaskDelete(NULL);
        }
        ESP_LOGI(TAG, "Succeed to reset GPIO channel : %d", i);
    }

    while (1) {
        for(int i = 0; i < 5; i++) {
            int raw = adc1_get_raw(flex_channels[i]);
            if (raw < 0) {
                ESP_LOGE(TAG, "ADC read fail ch %d", i);
                continue;
            }
            value_arr[i] = (uint16_t)raw;
        }
       
        if (xQueueSend(flexQueue, value_arr, pdMS_TO_TICKS(10)) != pdPASS) {
            ESP_LOGE(TAG, "failed to send xQueueSend"); 
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // 조절하면 글자 입력 시간 조절 가능
    }
}