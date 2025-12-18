#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "esp_rom_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"

// 샘플을 찍는 횟수 (올릴수록 정확도 향상)
#define SAMPLE_NUM 18
// 측정 기간
#define PERIOD_MS 100

// 손가락 마다 채널 부여
#define F1 ADC1_CHANNEL_0
#define F2 ADC1_CHANNEL_1
#define F3 ADC1_CHANNEL_2
#define F4 ADC1_CHANNEL_3
#define F5 ADC1_CHANNEL_4

static const char *TAG = "flex";

static const adc1_channel_t flex_channels[5] = { // 손가락 채널 배열
    F1, F2, F3, F4, F5
};

void adc_init(void)
{
    adc1_config_width(ADC_WIDTH_BIT_12); // 해상도 설정 (출력범위 0 ~ 4095)

    for (int i = 0; i < 5; i++) {
        adc1_config_channel_atten(
            flex_channels[i],
            ADC_ATTEN_DB_11 // 전압 범위 설정 3.3v (추후에 변경 가능)
        );
    }

    ESP_LOGI(TAG, "adc init done");
} 

void flex_get_value(void *pv)
{
    QueueHandle_t flex_queue = (QueueHandle_t)pv; // 외부에서 생성된 큐 핸들 수신

    uint16_t value_arr[5]; // 값 배열

    adc_init();

    while(1) {
        for (int i = 0; i < 5; i++) {
            int sum = 0;

            for (int j = 0; j < SAMPLE_NUM; j++) {
                int raw = adc1_get_raw(flex_channels[i]);

                if (raw < 0) {
                    ESP_LOGW(TAG, "adc read failed finger : %d", i);
                    continue;
                }

                sum += raw;
                esp_rom_delay_us(50); // vTaskdelay보다 더 짧은 delay (하드웨어 전용)
            }

            value_arr[i] = sum / SAMPLE_NUM; // 평균 값 저장
        }

        if (xQueueSend(flex_queue, value_arr, 0) != pdPASS) {
            ESP_LOGW(TAG, "queue data drop");
        }

        vTaskDelay(pdMS_TO_TICKS(PERIOD_MS));
    }
}