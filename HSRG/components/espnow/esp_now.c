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
#include "esp_now_set.h"
#include "esp_mac.h"
#include "esp_now.h"
#include "esp_crc.h"

static const char* TAG = "esp now";

QueueHandle_t espnowQueue = NULL;

/**
 * @brief init wifi func
 * @param[in] None
 * @retval None
 */
void wifi_init() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));  
}

/**
 * @brief receive the data for slave esp
 * @param[in] const uint8_t *mac_addr, const uint8_t *data, int data_len
 * @retval None
 * @note need to know MAC addr (and config)
 */
void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    espnow_event_t evt;
    evt.id = ESPNOW_RECV_CB;
    espnow_event_recv_cb_t *recv_cb = &evt.recv_cb;

    if (mac_addr == NULL || data == NULL || data_len <= 0) {
        ESP_LOGE(TAG, "too few arguments in Recv function call");
        return;
    }

    evt.id = ESPNOW_RECV_CB;
    memcpy(recv_cb->mac_addr, mac_addr, MAC_LEN); // 주소 복사
    recv_cb->data = malloc(data_len);

    if (recv_cb->data == NULL) {
        ESP_LOGE(TAG, "Malloc Recv data fail");
        return;
    }

    memcpy(recv_cb->data, data, data_len); // 데이터 복사
    recv_cb->data_len = data_len;

    if ((xQueueSend(espnowQueue, &evt, portMAX_DELAY) != pdPASS)) {
        ESP_LOGW(TAG, "esp now data failed to send");
        free(recv_cb->data); 
    }
}

/**
 * @brief parse the espnow snet data
 * @param[in] uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic
 * @retval int
 */
int espnow_data_parse(uint8_t *data, uint16_t data_len, uint8_t *state, uint16_t *seq, uint32_t *magic) {
    espnow_data_t *buf = (espnow_data_t *)data;
    uint16_t crc, crc_cal = 0;

    if (data_len < sizeof(espnow_data_t)) {
        ESP_LOGE(TAG, "Recv data is too short, LEN : %d", data_len);
        return -1;
    }

    *state = buf->state;
    *seq = buf->seq_num;
    *magic = buf->magic;
    crc = buf->crc;
    buf->crc = 0;
    crc_cal = esp_crc16_le(UINT16_MAX, (uint8_t const *)buf, data_len);

    if (crc_cal == crc) {
        return buf->type;
    }

    return -1;
}

/**
 * @brief ESP now main task
 * @param[in] void *pvParameter
 * @retval None
 */
void espnow_task(void *pvParameter) {
    while(1) {
        // 구현한 함수 호출 할 필요가 있음
    }
}

/**
 * @brief set up basic espnow config
 * @param[in] None
 * @retval esp_err_t
 * @note enroll the cb func to register 
 */
esp_err_t espnow_init(void) {
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    // set primary master key
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *)ESPNOW_PMK));
    
    esp_now_peer_info_t *peer = malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        vQueueDelete(espnowQueue);
        espnowQueue = NULL;
        espnow_deinit();
        return ESP_FAIL;
    }
}

static void espnow_deinit(void) {
    vQueueDelete(espnowQueue);
    espnowQueue = NULL;
    esp_now_deinit();
}

// https://github.com/espressif/esp-idf/blob/master/examples/wifi/espnow/main/espnow_example.h