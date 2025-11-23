#include "esp_log.h"
#include "esp_config_basic.h"

const static char* TAG = "RCV value";

/**
 * @brief Receive Master data
 * @param const uint8_t *sender_mac_addr, const my_data_t *data
 * @retval None
 */

void data_rcv(const uint8_t *sender_mac_addr, const my_data_t *data) {
    ESP_LOGI(TAG, "Data from "MACSTR": Random Value - %u, Button - %s",
                MAC2STR(sender_mac_addr), 
                data->random_value, 
                data->button_pushed ? "Pushed" : "Released");
}

