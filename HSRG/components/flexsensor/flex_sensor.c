#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"

#define CH0 ADC1_CHANNEL_0
#define CH1 ADC1_CHANNEL_1
#define CH2 ADC1_CHANNEL_2
#define CH3 ADC1_CHANNEL_3
#define CH4 ADC1_CHANNEL_4

static const char* TAG = "flex Sensor";

/** 
 * @brief flex sensor의 기본 설정 및 값을 얻어온 후 main에 queue로 전송한다    
 * @param void* pvParameters 로 Task 핸들을 받아온다
 * @retval flex sensor의 값을 리턴한다
*/
void flex_sensor_get_value(void* pvParameters) {
    static esp_err_t err;
    float value_arr[5] = { -1 };
    QueueHandle_t flexQueue = (QueueHandle_t)pvParameters;
    
    float flex_channels[5] = {CH0, CH1, CH2, CH3, CH4};

    err = adc1_config_width(ADC_WIDTH_12Bit); // 몇 비트 읽을지 결정, 높을수록 섬세함
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Fail to config adc width / Error name : %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "Succeed to config adc width");

    for(int i = 0; i < 5; i++) {
        err = adc1_config_channel_atten(flex_channels[i], ADC_ATTEN_11db); // 감쇠 정도 (flex sensor에 11decibel까지 줄여서 전달)
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Fail to reset GPIO channel : %d / Error name : %s", i, esp_err_to_name(err));
            return;
        }
        ESP_LOGI(TAG, "Succeed to reset GPIO channel : %d", i);
    }

    while(1) {
        for(int i = 0; i < 5; i++) {
            value_arr[i] = (float)adc1_get_raw(flex_channels[i]);

            if (value_arr[i] == -1) {
                ESP_LOGE(TAG, "failed to get %d : value", i);
                return;
            }
        }
       
        if (xQueueSend(flexQueue, value_arr, 0) != pdPASS) {
            ESP_LOGE(TAG, "failed to send xQueueSend");
        }

        vTaskDelay(pdMS_TO_TICKS(100)); // 조절하면 글자 입력 시간 조절 가능
    }
}