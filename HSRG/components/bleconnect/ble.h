#pragma once 

int gap_event_cb(struct ble_gap_event *event, void *arg);
void start_ad(void);
void ble_main_task(void);
void ble_app_on_sync(void);
void host_task(void *param);
void ble_send_string(const char* str);