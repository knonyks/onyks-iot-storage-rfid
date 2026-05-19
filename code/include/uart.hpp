#pragma once
#include "config.hpp"

#include "driver/uart.h"

class UART {
    public:
        uart_config_t uart_config;

        UART(int baud);

        void init(
            uart_port_t uart_num,
            QueueHandle_t& queue,
            int tx_pin, 
            int rx_pin,
            int rx_buff_size, 
            int tx_buff_size, 
            int queue_size
        );
};