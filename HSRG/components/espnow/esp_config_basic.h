#ifndef ESPNOW_BASIC_CONFIG_H
#define ESPNOW_BASIC_CONFIG_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct __attribute__((packed)) {
    uint32_t random_value;
    bool button_pushed;
} my_data_t;

#define MY_RECEIVER_MAC {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}

#define MY_ESPNOW_PMK "pmk1234567890123"
#define MY_ESPNOW_CHANNEL 1

#define MY_SLAVE_DEEP_SLEEP_TIME_MS 10000

#endif 