#include <stdio.h>
#include "flex_sensor.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"

void app_main(void)
{
    flex_sensor_get_value();
}   