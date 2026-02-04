idf_component_register(
    SRCS "main_core.c"
    INCLUDE_DIRS "."
    REQUIRES flexsensor espnow bleconnect driver nvs_flash calculate
)