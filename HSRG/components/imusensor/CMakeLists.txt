idf_component_register(
    SRCS "imu_sensor.c" "imu_sensor.cpp"
    INCLUDE_DIRS "."
    REQUIRES driver I2Cbus MPUdriver
)