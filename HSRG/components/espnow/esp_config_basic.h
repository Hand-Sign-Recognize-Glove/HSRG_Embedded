#pragma once;
#include "esp_now_set.h"

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF WIFI_IF_STA

// WiFi init channel
#define CONFIG_ESPNOW_CHANNEL 1

// MAC addr
#define MAC_LEN 6
#define ESPNOW_QUEUE_SIZE 6

#define ESPNOW_RECV_CB 1
#define ESPNOW_PMK "1fA9cD83eL02pQxZ"

#define ESPNOW_MAX_RECV_DATA 250

extern uint8_t peer_mac[6];

typedef struct {
    uint8_t mac_addr[MAC_LEN];
    uint8_t data[ESPNOW_MAX_RECV_DATA];
    int data_len;
} espnow_event_recv_cb_t;

typedef struct {
    uint32_t id;
    espnow_event_recv_cb_t recv_cb;
} espnow_event_t;

typedef struct {
    uint8_t type;
    uint8_t state;
    uint8_t seq_num;
    uint8_t crc;
    uint8_t magic;
    uint8_t payload[0];
} __attribute__((packed)) espnow_data_t;
