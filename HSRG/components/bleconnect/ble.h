#pragma once 

static int gap_event_cb(struct ble_gap_event *event, void *arg);
static void start_ad(void);
void ble_main_task(void);