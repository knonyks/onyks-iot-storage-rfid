#pragma once
#include "config.hpp"

#include "uart.hpp"
#include "R200.hpp"
#include "WS2812B.hpp"

struct SystemComponents {
    R200 rfid;
    //HA home_assistant;
    QueueHandle_t cmd_queue;
    QueueHandle_t data_queue;
};

class App {
    private:
        WS2812B led;
        UART uart2;
        SystemComponents sys_comp;

        esp_err_t create_tasks();
    public:
        App();
        void init();
        void exec();
};