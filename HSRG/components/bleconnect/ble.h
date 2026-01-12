#pragma once 
#include <stdint.h>
#include "host/ble_gap.h" 

/**
 * @brief Ble callback func
 * @param[in] struct ble_gap_event *event, void *arg
 * @retval int
 */
int gap_event_cb(struct ble_gap_event *event, void *arg);

/**
 * @brief start ble advertising
 * @param[in] None
 * @retval None 
 */
void start_ad(void);

/**
 * @brief send the string to phone
 * @param[in] const char* data
 * @retval None
 */
void ble_send_string(const char* str);

/**
 * @brief access cb func, convert true/false
 * @param[in] uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg
 * @retval int
 */
int chr_access_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

void ble_main_task(void* pvParameter);
void ble_app_on_sync(void);
void host_task(void *param);

