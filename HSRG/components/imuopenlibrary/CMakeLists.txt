idf_component_register(
        SRCS "mpu9250.c" "mpu9250_tests.c"
        REQUIRES driver log freertos)
target_include_directories(${COMPONENT_LIB} PUBLIC .)
target_include_directories(${COMPONENT_LIB} PUBLIC include)
set(driver_srcs
        "include/driver_mpu9250.c")
target_sources(${COMPONENT_LIB} PUBLIC "${driver_srcs}")
set(driver_interface_srcs
        "interface/driver_mpu9250_interface.c")
target_sources(${COMPONENT_LIB} PRIVATE "${driver_interface_srcs}")
