#pragma once

#define GPIO_SDA_NUM GPIO_NUM_21
#define GPIO_SCL_NUM GPIO_NUM_22
#define I2C_PORT_NUM I2C_NUM_0
#define IMU_SENSOR_LEN 1

typedef struct {   
    int16_t accel_raw[1][3];
    float   accel_g[1][3];
    int16_t gyro_raw[1][3];
    float   gyro_dps[1][3];
    int16_t mag_raw[1][3];
    float   mag_ut[1][3];
} imuData;

/**
 * @brief imu sensor init func
 * @param[in] None
 * @retval status code 
 *         - 0 success
 *         - 1 fail
 */
uint8_t imu_sensor_init(void);

/**
 * @brief imu sensor get data
 * @param[in] void* pvParameters
 * @retval None
 *  */
void imu_sensor_task(void* pvParameters);