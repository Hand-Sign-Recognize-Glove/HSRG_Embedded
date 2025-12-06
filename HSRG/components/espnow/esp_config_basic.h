#include <stdint.h>

#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF WIFI_IF_STA

// WiFi init channel
#define CONFIG_ESPNOW_CHANNEL 1

// MAC addr
#define MAC_LEN 6
#define ESPNOW_RECV_CB 1;

typedef struct {
    uint8_t mac_addr[MAC_LEN];
    uint8_t *data;
    int data_len;
} espnow_event_recv_cb_t;

typedef struct {
    uint32_t id;
    espnow_event_recv_cb_t recv_cb;
} espnow_event_t;