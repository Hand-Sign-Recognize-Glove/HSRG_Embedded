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


void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
    espnow_event_t evt;
    evt.id = ESPNOW_RECV_CB;
    espnow_event_recv_cb_t *recv_cb = &evt.recv_cb;
    uint8_t * now_mac = recv_cb->mac_addr;

    if (mac_addr == NULL || data == NULL || data_len <= 0) {
        ESP_LOGE(TAG, "too few arguments in Recv function call");
        return;
    }

    evt.id = ESPNOW_RECV_CB;
    memcpy(recv_cb->mac_addr, mac_addr, MAC_LEN);
    recv_cb->data = malloc(data_len);

    // Queue 전달 코드 짜기

}

void espnow_task(void *pvParameter) {
    while(1) {

    }
}

