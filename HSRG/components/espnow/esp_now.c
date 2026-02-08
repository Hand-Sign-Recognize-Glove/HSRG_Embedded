#include "esp_now_set.h"
#include "freertos/queue.h"
#include <inttypes.h>
#include "esp_config_basic.h"
#include "esp_now.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_random.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_crc.h"

static const char* TAG = "esp now";
QueueHandle_t espnowQueue = NULL;
TaskHandle_t espnowTaskHandle = NULL;

uint8_t peer_mac[6] = {0x94, 0x54, 0xc5, 0x73, 0x92, 0x90};

void wifi_init() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));

    ESP_LOGI(TAG, "Starting WiFi ...");
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "WiFi Started!");
}

void espnow_recv_cb_idf5(const esp_now_recv_info_t *recv_info, const uint8_t *data, int len) {
    espnow_recv_cb(recv_info->src_addr, data, len);
}   

void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    if (espnowQueue == NULL) {
        return;
    }

    if (mac_addr == NULL || data == NULL || data_len <= 0) {
        ESP_LOGE(TAG, "too few arguments in Recv function call");
        return;
    }
    
    espnow_event_t evt;
    evt.id = ESPNOW_RECV_CB;
    espnow_event_recv_cb_t *recv_cb = &evt.recv_cb;

    if (data_len > ESPNOW_MAX_RECV_DATA) {
        ESP_LOGW(TAG, "recv data too large");
        return;
    }

    memcpy(recv_cb->mac_addr, mac_addr, MAC_LEN); 
    memcpy(recv_cb->data, data, data_len);
    recv_cb->data_len = data_len;

    if ((xQueueSend(espnowQueue, &evt, 0) != pdPASS)) {
        ESP_LOGW(TAG, "esp now data failed to send");
    }
}

int espnow_data_parse(uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic) {
    if (!data || data_len < sizeof(espnow_data_t) || data_len > ESPNOW_MAX_RECV_DATA) {
        ESP_LOGE(TAG, "Recv data is too short, LEN : %d", data_len);
        return -1; 
    }

    uint8_t *buffer_copy = (uint8_t *)malloc(data_len);
    if (buffer_copy == NULL) {
        ESP_LOGE(TAG, "Memory allocation failed for CRC check");
        return -1;
    }
    memcpy(buffer_copy, data, data_len);

    espnow_data_t *tmp = (espnow_data_t *)buffer_copy;
    uint16_t crc_rx = tmp->crc;
    tmp->crc = 0; 

    uint16_t crc_cal = esp_crc16_le(UINT16_MAX, buffer_copy, data_len);
    
    free(buffer_copy);

    if (crc_cal != crc_rx) {
        return -1;
    }

    // 값 반환은 원본 데이터 포인터 대신 파싱된 값 사용
    // 주의: 여기서 tmp는 이미 해제된 메모리이므로, 원본 data를 캐스팅해서 값 읽음
    espnow_data_t *origin = (espnow_data_t *)data;
    *state = origin->state;
    *seq = origin->seq_num;
    *magic = origin->magic;

    return origin->type;
}

void espnow_task(void *pvParameter) {
    QueueHandle_t parsedQueue = (QueueHandle_t)pvParameter;  
    if (parsedQueue == NULL) {
        ESP_LOGE(TAG, "parsedQueue is NULL");
        vTaskDelete(NULL);
    }

    espnow_event_t evt;
    while (1) {
        if (espnowQueue == NULL) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        if (xQueueReceive(espnowQueue, &evt, portMAX_DELAY) != pdPASS) {
            continue;
        }

        uint8_t recv_state = 0;
        uint16_t recv_seq = 0;
        uint32_t recv_magic = 0;

        espnow_event_recv_cb_t *recv_cb = &evt.recv_cb;

        int type = espnow_data_parse(
            recv_cb->data,
            recv_cb->data_len,
            &recv_state,
            &recv_seq,
            &recv_magic
        );

        if (type >= 0) {
            if (xQueueSend(parsedQueue, &evt, 0) != pdPASS) {
                ESP_LOGW(TAG, "parsedQueue full");
            }
        }
    }
}
 
void espnow_init(void) { 
    espnowQueue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
    if (espnowQueue == NULL) {
        ESP_LOGE(TAG, "failed to Create new queue");
        return;
    }

    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb_idf5));

    // set primary master key
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *)ESPNOW_PMK));
    
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        vQueueDelete(espnowQueue);
        espnowQueue = NULL;
        espnow_deinit();
        return;
    }

    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_ESPNOW_CHANNEL; 
    peer->ifidx = ESPNOW_WIFI_IF; 
    peer->encrypt = false; 
    memcpy(peer->peer_addr, peer_mac, MAC_LEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);

    ESP_LOGI(TAG, "Initialization successful");
}

void espnow_deinit(void) {
    esp_err_t err;
    if (espnowTaskHandle) {
        vTaskDelete(espnowTaskHandle);
        espnowTaskHandle = NULL;
    }

    if (espnowQueue) {
        vQueueDelete(espnowQueue);
        espnowQueue = NULL;
    }

    err = esp_now_deinit();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "failed to deinit espnow");
        return;
    }
}