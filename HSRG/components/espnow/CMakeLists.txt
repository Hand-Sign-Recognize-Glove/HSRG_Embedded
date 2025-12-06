idf_component_register(
    SRCS "esp_now.c"
    PRIV_REQUIRES nvs_flash esp_event esp_netif esp_wifi
    INCLUDE_DIRS "."
)
