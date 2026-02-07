#include "cal.h"

static const char* TAG = "calculate";

// min, max 값 수정이 필요함
int min_standard[5] = {1600, 2500, 2500, 2500, 2300};
int max_standard[5] = {3000, 4000, 4000, 4000, 3800};

uint16_t flex_values[5] = { -1 };
float f_flex_values[5] = { -1 };
float f_imu_values[5] = { -1 };

int best_gesture_id = -1;
const char* best_gesture_name = "None";

imuData imuSensorData;  

Gesture_t gesture_consonant[14] = {
    {"ㄱ", 1, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㄴ", 2, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㄷ", 3, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㄹ", 4, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅁ", 5, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅂ", 6, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅅ", 7, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅇ", 8, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅈ", 9, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅊ", 10, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅋ", 11, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅌ", 12, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅍ", 13, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅎ", 14, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
};
Gesture_t gesture_vowels [10] = {
    {"ㅏ", 1, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅑ", 2, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅓ", 3, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅕ", 4, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅗ", 5, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅛ", 6, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅜ", 7, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅠ", 8, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅡ", 9, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
    {"ㅣ", 10, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}, {0.1f, 0.1f, 0.1f, 0.1f, 0.1f}},
};

void cal_func(float *flex_arr, float *imu_arr) {
    float min_error = 1e9;
    int find_idx = -1;
    int best_type = -1;

    for (int i = 0; i < 14; i++) {
        float current_error = 0.0f;

        for (int j = 0; j < 5; j++) {
            float diff = flex_arr[j] - gesture_consonant[i].standard_flex[j];
            current_error += diff * diff;
        }

        for (int k = 0; k < 3; k++) {
            float diff = imu_arr[k] - gesture_consonant[i].standard_imu[k];
            current_error += diff * diff;
        }

        if (current_error < min_error) {
            min_error = current_error;
            find_idx = i;
            best_type = 0;
        }
    }

    for (int i = 0; i < 10; i++) {
        float current_error = 0.0f;

        for (int j = 0; j < 5; j++) {
            float diff = flex_arr[j] - gesture_vowels[i].standard_flex[j];
            current_error += diff * diff;
        }

        for (int k = 0; k < 3; k++) {
            float diff = imu_arr[k] - gesture_vowels[i].standard_imu[k];
            current_error += diff * diff;
        }

        if (current_error < min_error) {
            min_error = current_error;
            find_idx = i;
            best_type = 1;
        }
    }

    if (find_idx != -1 && min_error < 1e9 && best_type != -1) { //값 조정
        if (best_type == 0) {
            best_gesture_id = gesture_consonant[find_idx].id;
            best_gesture_name = gesture_consonant[find_idx].letter;
        }
        else if (best_type == 1) {
            best_gesture_id = gesture_vowels[find_idx].id;
            best_gesture_name = gesture_vowels[find_idx].letter;
        }
        ESP_LOGI(TAG, "letter : %s, letter id : %d", best_gesture_name, best_gesture_id);
    }
    else {
        ESP_LOGE(TAG, "Can not found letter");
    }
    
}

float convert_to_float(int value, int min_val, int max_val) {
    if (value <= min_val) return 0.0f; 
    if (value >= max_val) return 1.0f;

    return (float)(value - min_val) / (float)(max_val - min_val);
}

void main_cal_task(void* pvParameters) {
    while (flexQueue == NULL || imuQueue == NULL) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    while(1) {
        if (xQueueReceive(flexQueue, flex_values, portMAX_DELAY) == pdPASS) {
            for(int i = 0; i < 5; i++) {
                f_flex_values[i] = convert_to_float(flex_values[i], min_standard[i], max_standard[i]);
            }
            ESP_LOGW(TAG, "%f, %f, %f, %f, %f", f_flex_values[0], f_flex_values[1], f_flex_values[2], f_flex_values[3], f_flex_values[4]);
        }

        bool imu_updated = false;
        while (xQueueReceive(imuQueue, &imuSensorData, 0) == pdPASS) {
            imu_updated = true;
        } 

        if (imu_updated) {
            ESP_LOGI(TAG, "ACC: %.2f %.2f %.2f  GYR: %.2f %.2f %.2f",
            imuSensorData.accel_g[0][0], imuSensorData.accel_g[0][1],
            imuSensorData.accel_g[0][2], imuSensorData.gyro_dps[0][0],
            imuSensorData.gyro_dps[0][1], imuSensorData.gyro_dps[0][2]);
            for (int i = 0; i < 3; i++) {
                f_imu_values[i] = convert_to_float(imuSensorData.accel_g[0][0], imuSensorData.accel_g[0][1], imuSensorData.accel_g[0][2]);
            }
        }
        cal_func(f_flex_values, f_imu_values);
    }
}

