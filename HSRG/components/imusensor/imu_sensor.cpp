extern "C" {
    #include "imu_sensor.h"
}

#include "esp_log.h"
#include "MPU.hpp"
#include "mpu/types.hpp"
#include "mpu/math.hpp"
#include "I2Cbus.hpp"

#define SDA 21 // 핀 알맞게 설정
#define SCL 22

static const char* TAG = "imu bridgge";

using namespace mpud;

extern I2C_t i2c0;
static MPU_t s_mpu;

/** 
 * @brief init to imu sensor
 * @param[in] None
 * @retval ESP_OK, ESP_FAIL
 * @details I2C를 이용한 IMU 초기화 함수
*/
extern "C" esp_err_t imu_init() {
    esp_err_t res;

    i2c0.begin((gpio_num_t)SDA, (gpio_num_t)SCL, 400000); // 주파수 설정 및 드라이버 초기화

    s_mpu.setBus(i2c0); // i2c 버스 저장
    s_mpu.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  

    res = s_mpu.testConnection();
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "failed to test connect to i2c \n adress : 0x%x", res);
        return res;
    }

    ESP_ERROR_CHECK(s_mpu.initialize());

    s_mpu.setSampleRate(100); // 새로운 클럭 주기 생성
    s_mpu.setAccelFullScale(mpud::ACCEL_FS_4G); // 가속도 범위 설정
    s_mpu.setGyroFullScale(mpud::GYRO_FS_500DPS); // 자이로 범위 생성
    s_mpu.setDigitalLowPassFilter(mpud::DLPF_42HZ); // 노이즈 제거 및 값 안정화mpud::raw_axes_t accelR

    ESP_LOGI(TAG, "Succeed to init mpu");
    return ESP_OK;
}

/**
 * @brief get imu sensor value
 * @param[in] float* ax, float* ay, float* az, float* gx, float*gy, float* gz
 * @retval ESP_OK, ESP_FAIL
 * @details imu sensor의 값을 받아 저장하는 함수
 */
extern "C" esp_err_t imu_get(float* ax, float* ay, float* az, float* gx, float*gy, float* gz) {
    mpud::raw_axes_t accelRaw;
    mpud::raw_axes_t gyroRaw;

    esp_err_t res;

    res = s_mpu.acceleration(&accelRaw);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "failed to get accelRaw");
        return res;
    }

    res = s_mpu.rotation(&gyroRaw);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "failed to get gyroRaw");
        return res;
    }
    ESP_LOGI(TAG, "Succeed to get accelRaw and gyroRaw");

    auto accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);
    auto gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);

    *ax = accelG.x;
    *ay = accelG.y;
    *az = accelG.z;

    *gx = gyroDPS.x;
    *gy = gyroDPS.y;
    *gz = gyroDPS.z;

    return ESP_OK;
}