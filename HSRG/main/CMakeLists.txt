idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES flexsensor espnow bleconnect driver nvs
)