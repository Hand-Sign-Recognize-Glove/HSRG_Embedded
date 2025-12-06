#pragma once
#include "freertos/queue.h"

#include <inttypes.h>
#include "esp_config_basic.h"

// espnowQueue 외부 변수 선언
extern QueueHandle_t espnowQueue;

void wifi_init();
void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len);
void espnow_task(void *pvParameter);
esp_err_t espnow_init(void);