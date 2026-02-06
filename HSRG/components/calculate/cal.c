#include "cal.h"

static const char* TAG = "calculate";

// min, max 값 수정이 필요함
int min_standard[5] = {1600, 2500, 2500, 2500, 2300};
int max_standard[5] = {3000, 4000, 4000, 4000, 3800};

uint16_t flex_values[5] = { -1 };
float f_flex_values[5] = { -1 };

void cal_func(float *flex_arr) {
    return;
}

float convert_to_float(int value, int min_val, int max_val) {
    if (value <= min_val) return 0.0f; 
    if (value >= max_val) return 1.0f;

    return (float)(value - min_val) / (float)(max_val - min_val);
}

void main_cal_task(void* pvParameters) {
    while (flexQueue == NULL) {
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    while(1) {
        if (xQueueReceive(flexQueue, flex_values, portMAX_DELAY) == pdPASS) {
            for(int i = 0; i < 5; i++) {
                f_flex_values[i] = convert_to_float(flex_values[i], min_standard[i], max_standard[i]);
            }
            ESP_LOGW(TAG, "%f, %f, %f, %f, %f", 
                f_flex_values[0], 
                f_flex_values[1], 
                f_flex_values[2], 
                f_flex_values[3], 
                f_flex_values[4]);
            cal_func(f_flex_values);
        }
        else {
            ESP_LOGE(TAG, "failed to recv flex Queue");
            ESP_LOGW(TAG, "Waiting to recv flex Queue");
            vTaskDelay(100);
        }
    }
}

