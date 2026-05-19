#include "uart.hpp"

UART::UART(int baud) : uart_config{} {
    uart_config.baud_rate = baud;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;
}

void UART::init(
    uart_port_t uart_num,
    QueueHandle_t& queue,
    int tx_pin, 
    int rx_pin,
    int rx_buff_size, 
    int tx_buff_size, 
    int queue_size
) {

    ESP_ERROR_CHECK(
        uart_param_config(
            uart_num, 
            &uart_config
        )
    );

    ESP_ERROR_CHECK(
        uart_set_pin(
            uart_num, 
            tx_pin, 
            rx_pin, 
            UART_PIN_NO_CHANGE, 
            UART_PIN_NO_CHANGE
        )
    );

    ESP_ERROR_CHECK(
        uart_driver_install(
            uart_num, 
            rx_buff_size, 
            tx_buff_size, 
            queue_size, 
            &queue, 0
        )
    );
}