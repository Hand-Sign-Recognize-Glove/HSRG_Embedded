#include <stdio.h>
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/ble_gap.h"
#include "esp_nimble_hci.h"
#include "esp_log.h"

static const char* TAG = "BLE protocol";
static uint8_t own_addr_type;

/**
 * @brief Ble callback func
 * @param[in] struct ble_gap_event *event, void *arg
 * @retval int
 */
int gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
    
    case BLE_GAP_EVENT_CONNECT:
        if(event->connect.status == 0) {
            ESP_LOGI(TAG, "ble is connected");
        }
        else {
            ESP_LOGE(TAG, "Connect failed \n retrying");
            ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, NULL, gap_event_cb, NULL);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI(TAG, "Disconnected \n restarting advertising");
        ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, NULL, gap_event_cb, NULL);

        break;

    default:
        break;
    }
    return 0;
}

/**
 * @brief start ble advertising
 * @param[in] None
 * @retval None 
 */
void start_ad(void) {
    struct ble_gap_adv_params adv_params = { 0 };
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    struct ble_hs_adv_fields fields = { 0 };
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)"HSRG ble";
    fields.name_len = strlen("HSRG ble");
    fields.name_is_complete = 1;

    ble_gap_adv_set_fields(&fields);

    ESP_LOGI(TAG, "Start adv");
    ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, NULL, gap_event_cb, NULL);
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(0x180F), 
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID16_DECLARE(0x2A19), 
                .access_cb = NULL,
                .flags = BLE_GATT_CHR_F_READ,
            }, 
            { 0 } 
        } 
    }, 
    { 0 } 
};

void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &own_addr_type);
    start_ad();
}

void host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}


void ble_main_task(void) {
    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    nimble_port_init();

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);
    nimble_port_freertos_init(host_task);
}