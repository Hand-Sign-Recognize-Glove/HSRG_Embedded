/**
 *
 * Based on https://github.com/libdriver/mpu9250/blob/master/interface/driver_mpu9250_interface_template.c
 *
 */

#include "driver_mpu9250_interface.h"
#include "include/driver_mpu9250.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *MPU9250_INTERFACE_TAG = "driver_mpu9250_interface";

#define I2C_READ_TIMEOUT_MS 1000
#define I2C_WRITE_TIMEOUT_MS 1000

i2c_master_bus_handle_t i2c_master_bus_hdl;
i2c_master_dev_handle_t i2c_master_mpu9250_dev_hdl;
i2c_master_dev_handle_t i2c_master_ak8963_dev_hdl;
i2c_master_bus_config_t *i2c_master_conf = NULL;
i2c_device_config_t *i2c_mpu9250_dev_conf = NULL;
i2c_device_config_t *i2c_ak8963_dev_conf = NULL;

esp_err_t mpu9250_set_i2c_config(i2c_master_bus_config_t *master_cfg, i2c_device_config_t *mpu9250_dev_conf) {
    i2c_master_conf = master_cfg;
    i2c_mpu9250_dev_conf = mpu9250_dev_conf;
    i2c_device_config_t tmp_i2c_dev_conf = {
            .scl_speed_hz = (*i2c_mpu9250_dev_conf).scl_speed_hz,
            .dev_addr_length = I2C_ADDR_BIT_LEN_7,
            .device_address = AK8963_IIC_ADDRESS
    };
    i2c_ak8963_dev_conf = malloc(sizeof(i2c_device_config_t));
    if(!i2c_ak8963_dev_conf) {
        ESP_LOGI(MPU9250_INTERFACE_TAG, "malloc failed for ak8963 i2c dev conf!");
        return ESP_FAIL;
    }
    *i2c_ak8963_dev_conf = tmp_i2c_dev_conf;
    return ESP_OK;
}


/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t mpu9250_interface_iic_init(void) {
    if (i2c_master_conf == NULL) {
        ESP_LOGE(MPU9250_INTERFACE_TAG, "i2c_master_config is null!");
        return 1;
    }

    esp_err_t ret;

    ret = i2c_new_master_bus(i2c_master_conf, &i2c_master_bus_hdl);
    if(ret != ESP_OK) {
        ESP_LOGE(MPU9250_INTERFACE_TAG, "i2c_new_master_bus error!");
        return 1;
    }
    ret = i2c_master_bus_add_device(i2c_master_bus_hdl, i2c_mpu9250_dev_conf, &i2c_master_mpu9250_dev_hdl);
    if(ret != ESP_OK) {
        ESP_LOGE(MPU9250_INTERFACE_TAG, "i2c_master_bus_add_device [mpu9250] error!");
        return 1;
    }
    ret = i2c_master_bus_add_device(i2c_master_bus_hdl, i2c_ak8963_dev_conf, &i2c_master_ak8963_dev_hdl);
    if(ret != ESP_OK) {
        ESP_LOGE(MPU9250_INTERFACE_TAG, "i2c_master_bus_add_device [ak8963] error!");
        return 1;
    }

    ESP_LOGI(MPU9250_INTERFACE_TAG, "mpu9250_interface_iic_init OK :)");

    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t mpu9250_interface_iic_deinit(void) {
    esp_err_t ret;
    ret = i2c_master_bus_rm_device(i2c_master_mpu9250_dev_hdl);
    assert(ESP_OK == ret);
    ret = i2c_master_bus_rm_device(i2c_master_ak8963_dev_hdl);
    assert(ESP_OK == ret);
    ret = i2c_del_master_bus(i2c_master_bus_hdl);
    assert(ESP_OK == ret);
    i2c_master_conf = NULL;
    i2c_mpu9250_dev_conf = NULL;
    if (ret != ESP_OK) {
        return 1;
    }
    free(i2c_ak8963_dev_conf);
    return 0;
}

/**
 * @brief      interface iic bus read
 * @param[in]  addr is the iic device write address
 * @param[in]  reg is the iic register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t mpu9250_interface_iic_read(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    esp_err_t ret;
    ESP_LOGD(MPU9250_INTERFACE_TAG, "mpu9250_interface_iic_read() | addr: %x, reg: %x, buff: %s, len: %d", addr, reg,buf, len);
    i2c_master_dev_handle_t i2c_dev_hdl = i2c_master_mpu9250_dev_hdl;
    if(addr == AK8963_IIC_ADDRESS) {
        i2c_dev_hdl = i2c_master_ak8963_dev_hdl;
    }
    ret = i2c_master_transmit_receive(i2c_dev_hdl, &reg, 1, buf, len, I2C_READ_TIMEOUT_MS);
    assert(ESP_OK == ret);
    if (ret != ESP_OK) {
        return 1;
    }
    return 0;
}

/**
 * @brief     interface iic bus write
 * @param[in] addr is the iic device write address
 * @param[in] reg is the iic register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t mpu9250_interface_iic_write(uint8_t addr, uint8_t reg, uint8_t *buf, uint16_t len) {
    esp_err_t ret;
    ESP_LOGD(MPU9250_INTERFACE_TAG, "mpu9250_interface_iic_write() | addr: %x, reg: %x, buff: %s, len: %d", addr, reg,buf, len);
    i2c_master_dev_handle_t i2c_dev_hdl = i2c_master_mpu9250_dev_hdl;
    if(addr == AK8963_IIC_ADDRESS) {
        i2c_dev_hdl = i2c_master_ak8963_dev_hdl;
    }
    uint8_t write_buf[len+1];
    write_buf[0]=reg;
    for(int i=1;i<len+1;i++){
        write_buf[i] = buf[i-1];
    }
    ret = i2c_master_transmit(i2c_dev_hdl, write_buf, sizeof(write_buf), I2C_WRITE_TIMEOUT_MS);
    assert(ESP_OK == ret);
    if (ret != ESP_OK) {
        return 1;
    }
    return 0;
}

/**
 * @brief  interface spi bus init
 * @return status code
 *         - 0 success
 *         - 1 spi init failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_init(void) {
    ESP_LOGE(MPU9250_INTERFACE_TAG, "MPU9250 SPI functions not yet implemented!");
    return 1;
//    return 0;
}

/**
 * @brief  interface spi bus deinit
 * @return status code
 *         - 0 success
 *         - 1 spi deinit failed
 * @note   none
 */
uint8_t mpu9250_interface_spi_deinit(void) {
    ESP_LOGE(MPU9250_INTERFACE_TAG, "MPU9250 SPI functions not yet implemented!");
    return 1;
//    return 0;
}

/**
 * @brief      interface spi bus read
 * @param[in]  reg is the register address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t mpu9250_interface_spi_read(uint8_t reg, uint8_t *buf, uint16_t len) {
    ESP_LOGE(MPU9250_INTERFACE_TAG, "MPU9250 SPI functions not yet implemented!");
    return 1;
//    return 0;
}

/**
 * @brief     interface spi bus write
 * @param[in] reg is the register address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t mpu9250_interface_spi_write(uint8_t reg, uint8_t *buf, uint16_t len) {
    ESP_LOGE(MPU9250_INTERFACE_TAG, "MPU9250 SPI functions not yet implemented!");
    return 1;
//    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void mpu9250_interface_delay_ms(uint32_t ms) {
    ESP_LOGD(MPU9250_INTERFACE_TAG,"pause for %ld ms...", ms);
    // esp_rom_delay_us(1000*ms);
    vTaskDelay(pdMS_TO_TICKS(ms));

}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void mpu9250_interface_debug_print(const char *const fmt, ...) {
    va_list args;
    va_start(args, fmt);
    esp_log_writev(ESP_LOG_DEBUG, MPU9250_INTERFACE_TAG, fmt, args);
    va_end(args);
}

/**
 * @brief     interface receive callback
 * @param[in] type is the irq type
 * @note      none
 */
void mpu9250_debug_receive_callback(uint8_t type) {
    switch (type) {
        case MPU9250_INTERRUPT_MOTION : {
            mpu9250_interface_debug_print("mpu9250: irq motion.\n");

            break;
        }
        case MPU9250_INTERRUPT_FIFO_OVERFLOW : {
            mpu9250_interface_debug_print("mpu9250: irq fifo overflow.\n");

            break;
        }
        case MPU9250_INTERRUPT_FSYNC_INT : {
            mpu9250_interface_debug_print("mpu9250: irq fsync int.\n");

            break;
        }
        case MPU9250_INTERRUPT_DMP : {
            mpu9250_interface_debug_print("mpu9250: irq dmp\n");

            break;
        }
        case MPU9250_INTERRUPT_DATA_READY : {
            mpu9250_interface_debug_print("mpu9250: irq data ready\n");

            break;
        }
        default : {
            mpu9250_interface_debug_print("mpu9250: irq unknown code.\n");

            break;
        }
    }
}

/**
 * @brief     interface dmp tap callback
 * @param[in] count is the tap count
 * @param[in] direction is the tap direction
 * @note      none
 */
void mpu9250_debug_dmp_tap_callback(uint8_t count, uint8_t direction) {
    switch (direction) {
        case MPU9250_DMP_TAP_X_UP : {
            mpu9250_interface_debug_print("mpu9250: tap irq x up with %d.\n", count);

            break;
        }
        case MPU9250_DMP_TAP_X_DOWN : {
            mpu9250_interface_debug_print("mpu9250: tap irq x down with %d.\n", count);

            break;
        }
        case MPU9250_DMP_TAP_Y_UP : {
            mpu9250_interface_debug_print("mpu9250: tap irq y up with %d.\n", count);

            break;
        }
        case MPU9250_DMP_TAP_Y_DOWN : {
            mpu9250_interface_debug_print("mpu9250: tap irq y down with %d.\n", count);

            break;
        }
        case MPU9250_DMP_TAP_Z_UP : {
            mpu9250_interface_debug_print("mpu9250: tap irq z up with %d.\n", count);

            break;
        }
        case MPU9250_DMP_TAP_Z_DOWN : {
            mpu9250_interface_debug_print("mpu9250: tap irq z down with %d.\n", count);

            break;
        }
        default : {
            mpu9250_interface_debug_print("mpu9250: tap irq unknown code.\n");

            break;
        }
    }
}

/**
 * @brief     interface dmp orient callback
 * @param[in] orientation is the dmp orientation
 * @note      none
 */
void mpu9250_debug_dmp_orient_callback(uint8_t orientation) {
    switch (orientation) {
        case MPU9250_DMP_ORIENT_PORTRAIT : {
            mpu9250_interface_debug_print("mpu9250: orient irq portrait.\n");

            break;
        }
        case MPU9250_DMP_ORIENT_LANDSCAPE : {
            mpu9250_interface_debug_print("mpu9250: orient irq landscape.\n");

            break;
        }
        case MPU9250_DMP_ORIENT_REVERSE_PORTRAIT : {
            mpu9250_interface_debug_print("mpu9250: orient irq reverse portrait.\n");

            break;
        }
        case MPU9250_DMP_ORIENT_REVERSE_LANDSCAPE : {
            mpu9250_interface_debug_print("mpu9250: orient irq reverse landscape.\n");

            break;
        }
        default : {
            mpu9250_interface_debug_print("mpu9250: orient irq unknown code.\n");

            break;
        }
    }
}