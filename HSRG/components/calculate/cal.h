#pragma once 

#include <stdio.h>
#include "esp_log.h"
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "main_core.h"

/**
 * @brief 글자 계산 로직
 * @param void
 * @retval[in] None
 */
void cal_func(void);

/**
 * @brief 자료형을 float로 변환
 * @param 
 *      - int value : now flexsensor[i] 
 *      - int min_val : fingers[i] min value
 *      - int max_val : fingers[i] max value
 * @retval float value
 */
float convert_to_float(int value, int min_val, int max_val);

/**
 * @brief 계산 메인 태스크
 * @param void* pvParameters
 * @retval[in] None
 */
void main_cal_task(void* pvParameters);