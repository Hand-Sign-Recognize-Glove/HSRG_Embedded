#pragma once 

#include <stdio.h>
#include "esp_log.h"
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "main_core.h"

void cal_func(void);
float convert_to_float(int value, int min_val, int max_val);
void main_cal_task(void* pvParameters);