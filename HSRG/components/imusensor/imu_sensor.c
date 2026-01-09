#include <stdio.h>
#include "esp_log.h"
#include "mpu9250.h"
#include "driver_mpu9250.h"
#include "mpu9250_tests.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
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

void imu_sensor_init() {
    esp_err_t err;
    
    err = mpu9250_init(&mpu9250_hdl);
    if (!err) {
        ESP_LOGE(TAG, "failed to init mpu");
        return;
    }

    ESP_ERROR_CHECK(mpu9250_init_i2c(&mpu9250_hdl, &i2c_master_conf, &i2c_dev_conf));
    ESP_LOGI(TAG, "succeed to mpu init i2c module");
}