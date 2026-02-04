#pragma once 

#include <stdio.h>
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/adc.h"
#include "esp_now_set.h"
#include "nvs_flash.h"
#include "ble.h"
#include "cal.h"

void main_core(void *pvParameters);
