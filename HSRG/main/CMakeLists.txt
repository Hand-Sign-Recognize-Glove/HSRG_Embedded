idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES flexsensor I2Cbus imusensor MPUdriver
)