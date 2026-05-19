#pragma once

#include <cstdint>
#include <vector>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"

#define LED_PIN                 2

#define R200_UART               UART_NUM_2
#define R200_BAUD               115200
#define R200_RX_PIN             16
#define R200_TX_PIN             17
#define R200_RX_BUFF_SIZE       512
#define R200_TX_BUFF_SIZE       0

#define WIFI_EVENT_QUEUE_SIZE   8
#define CMD_EVENT_QUEUE_SIZE    8
#define UART_EVENT_QUEUE_SIZE   24
#define DATA_EVENT_QUEUE_SIZE   24

#define TAG_LED                 "WS2812B"
#define TAG_R200                "R200"

enum rfid_event_type_t : uint8_t {
    IDLE_STATUS = 0x00, 
    TAG_READ = 0x01,
    ERROR_INFO = 0x02
};

struct rfid_event_t {
    rfid_event_type_t type;
    uint8_t rssi;
    uint8_t data[24];
    uint8_t data_len;
};

struct ha_event_t {
    uint8_t cmd;
    uint8_t params[16];
    uint8_t param_len;
};