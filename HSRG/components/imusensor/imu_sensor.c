#include <stdio.h>
#include "esp_log.h"
#include "mpu9250.h"
#include "driver_mpu9250.h"
#include "mpu9250_tests.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "imu_sensor.h"

static const char *TAG = "IMU sensor";
static mpu9250_handle_t mpu9250_hdl;

i2c_master_bus_config_t i2c_master_conf = {
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .i2c_port = I2C_PORT_NUM,
    .sda_io_num = GPIO_SDA_NUM,
    .scl_io_num = GPIO_SCL_NUM,
    .flags.enable_internal_pullup = true,
    .glitch_ignore_cnt = 9,
};

i2c_device_config_t i2c_dev_conf = {
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = 0x68,
            .scl_speed_hz = 400000,
};

void imu_sensor_task(void* pvParameters) {
    int8_t err = imu_sensor_init();
    if (err != 0) {
        ESP_LOGE(TAG, "Failed to init imu sensor");
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG, "Success to init imu sensor");

    imuData data;
    uint16_t len = 1;
    QueueHandle_t imuQueue = (QueueHandle_t)pvParameters;
    
    while (1) {
        uint8_t ret = mpu9250_read(&mpu9250_hdl, data.accel_raw, data.accel_g, data.gyro_raw, data.gyro_dps, data.mag_raw, data.mag_ut, &len);

        if (ret != 0) {
            ESP_LOGE(TAG,  "failed to get imu data (%d)", ret);
            vTaskDelete(NULL);
        }
        if (xQueueSend(imuQueue, &data, pdMS_TO_TICKS(1)) != pdPASS) {
            ESP_LOGE(TAG, "failed to send xQueueSend");
        }
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

uint8_t imu_sensor_init() {
    uint8_t err;

    ESP_ERROR_CHECK(mpu9250_init_i2c(&mpu9250_hdl, &i2c_master_conf, &i2c_dev_conf));
    
    err = mpu9250_init(&mpu9250_hdl);
    if (err != 0) {
        ESP_LOGE(TAG, "failed to init mpu (%d)", err);
        return 1; 
    }

    ESP_LOGI(TAG, "succeed to mpu init i2c module");
    return 0;
}