#pragma oncee

#include <inttypes.h>
#include "esp_config_basic.h"

void esp_now_master(void);
void data_rcv(const uint8_t *sender_mac_addr, const my_data_t *data);   