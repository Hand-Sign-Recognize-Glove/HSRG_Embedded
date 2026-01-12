#include <stdio.h>
#include <string.h>

#include "esp_log.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_uuid.h"

#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char* TAG = "BLE protocol";
static uint8_t own_addr_type;
static uint16_t g_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t g_chr_handle;
static bool notify_enabled = false;

#define MY_SERVICE_UUID 0xFFF0
#define MY_CHAR_UUID 0xFFF1

int gap_event_cb(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
    
    case BLE_GAP_EVENT_CONNECT:
        if(event->connect.status == 0) {
            g_conn_handle = event->connect.conn_handle;
            ESP_LOGI(TAG, "ble is connected");
        }
        else {
            ESP_LOGE(TAG, "Connect failed \n retrying");
            ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, NULL, gap_event_cb, NULL);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        g_conn_handle = BLE_HS_CONN_HANDLE_NONE;
        notify_enabled = false;

        ESP_LOGI(TAG, "Disconnected \n restarting advertising");
        ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, NULL, gap_event_cb, NULL);

        break;

    default:
        break;
    }
    return 0;
}

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

void ble_send_string(const char* data) {
    if (g_conn_handle == BLE_HS_CONN_HANDLE_NONE) return;
    if (!notify_enabled) return;

    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, strlen(data));
    if (!om) {
        ESP_LOGE(TAG, "failed to copy mbuf");
        return;
    }

    ble_gatts_notify_custom(g_conn_handle, g_chr_handle, om);
}

int chr_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_DSC) {
        uint16_t v = ctxt->om->om_data[1] << 8 | ctxt->om->om_data[0];
        notify_enabled = (v == 0x0001);
    }
    else {
        ESP_LOGW(TAG, "bad request");
    }
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(MY_SERVICE_UUID), 
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                .uuid = BLE_UUID16_DECLARE(MY_CHAR_UUID), 
                .access_cb = chr_access_cb,
                .flags = BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &g_chr_handle,
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

void ble_main_task(void* pvParameter) {
    nimble_port_init();

    ble_hs_cfg.sync_cb = ble_app_on_sync;
    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);
    nimble_port_freertos_init(host_task);
}