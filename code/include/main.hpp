#ifndef MAINHPP
#define MAINHPP

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"
#include "esp_log.h"

#define GPIO_LED 2

void led_init(led_strip_handle_t *led);

#endif