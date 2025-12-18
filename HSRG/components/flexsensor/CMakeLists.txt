idf_component_register(
    SRCS "flex_sensor.c"
    INCLUDE_DIRS "."
    REQUIRES driver freerots
    PRIV_REQUIRES driver log
)