#include <stdio.h>
#include "flex_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

static const char *TAG = "main";

void app_main(void)
{
    QueueHandle_t flex_queue;

    flex_queue = xQueueCreate( // 큐 생성
        5,
        sizeof(uint16_t) * 5
    );

    if (flex_queue == NULL){
        ESP_LOGI(TAG, "queue create failed"); // 예외 처리
        return;
    }

    xTaskCreate( // 테스크 생성
        flex_get_value,
        "flex_task",
        4096,
        (void *)flex_queue, // flex_get_value 파라미터(테스크 함수라 이렇게 사용)
        5,
        NULL
    );
}   