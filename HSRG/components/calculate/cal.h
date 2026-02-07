#pragma once 

#include <stdio.h>
#include "esp_log.h"
#include "flex_sensor.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "main_core.h"

typedef struct {
    const char *letter;
    int id;
    float standard_flex[5];
    float standard_imu[5];
} Gesture_t;

extern Gesture_t gesture_consonant[14];
extern Gesture_t gesture_vowels [10];

/**
 * @brief 글자 계산 로직
 * @param float *flex_arr, imuData imu
 * @retval[in] None
 */
void cal_func(float *flex_arr, float *imu_arr);

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