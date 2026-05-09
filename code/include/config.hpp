#pragma once

#include <cstdint>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define LED_PIN                 2
#define R200_UART               UART_NUM_2
#define R200_BAUD               115200
#define R200_RX_PIN             16
#define R200_TX_PIN             17
#define R200_RX_BUFF_SIZE       512
#define R200_TX_BUFF_SIZE       0
#define R200_EVENT_QUEUE_SIZE   6

#define TAG_LED                 "WS2812B"
#define TAG_R200                "R200"
