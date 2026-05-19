#include "App.hpp"

App::App() : led{}, uart2{R200_BAUD}, sys_comp{} {
    sys_comp.rfid = R200{};
    // sys_comp.home_assistant = HA{};
    sys_comp.cmd_queue = xQueueCreate(CMD_EVENT_QUEUE_SIZE, sizeof(ha_event_t));
    sys_comp.data_queue = xQueueCreate(DATA_EVENT_QUEUE_SIZE, sizeof(rfid_event_t));
};

void App::init() {
    led.init();
    uart2.init(
        R200_UART,
        sys_comp.rfid.uart_queue,
        R200_TX_PIN, 
        R200_RX_PIN, 
        R200_RX_BUFF_SIZE, 
        R200_TX_BUFF_SIZE, 
        UART_EVENT_QUEUE_SIZE
    );
    sys_comp.rfid.init();
}

void App::exec() {
    // settings
    led.set_colour(0, 150, 0);

    // start up
    create_tasks();
    vTaskStartScheduler();

    // main loop
    while (1) {
        led.toggle();
        vTaskDelay(pdMS_TO_TICKS(500));
        led.toggle();
        vTaskDelay(pdMS_TO_TICKS(4000));
    }
}

esp_err_t App::create_tasks() {
    // do zrobienia
    #pragma region Home Assistant command receiver
    auto ha_cmd_receiver = [](void* pvParameters) {
        
        SystemComponents* sys_comp = static_cast<SystemComponents*>(pvParameters);

        while (1) { 
            // Wait for command from HA task
            // if command received, send to R200 task via queue
        }
    };
    
    xTaskCreate(
        ha_cmd_receiver,
        "HA Command Receiver",
        2048,
        &sys_comp,
        16,
        NULL
    );
    #pragma endregion
    // gotowy
    #pragma region RFID command sender
    auto rfid_cmd_sender = [](void* pvParameters) {
        ha_event_t event{};
        SystemComponents* sys_comp = static_cast<SystemComponents*>(pvParameters);

        while (1) { 
            if (xQueueReceive(sys_comp->cmd_queue, &event, portMAX_DELAY))
                sys_comp->rfid.send_cmd(event);
        }
    };
    
    xTaskCreate(
        rfid_cmd_sender,
        "R200 Command Sender",
        2048,
        &sys_comp,
        12,
        NULL
    );
    #pragma endregion
    // gotowy
    #pragma region RFID data receiver
    auto rfid_data_rec = [](void* pvParameters) {
        uart_event_t event{};
        SystemComponents* sys_comp = static_cast<SystemComponents*>(pvParameters);       

        while (1) { 
            if(xQueueReceive(sys_comp->rfid.uart_queue, &event, portMAX_DELAY))
                if (event.type == UART_DATA)
                    sys_comp->rfid.receive_data();
        }
    };

    xTaskCreate(
        rfid_data_rec,
        "R200 Data Receiver",
        2048,
        &sys_comp,
        14,
        NULL
    );
    #pragma endregion
    // do zrobienia
    #pragma region Home Assistant data sender
    auto ha_data_rec = [](void* pvParameters) {
        rfid_event_t event{};
        SystemComponents* sys_comp = static_cast<SystemComponents*>(pvParameters);

        while (1) {
            // if (xQueueReceive(sys_comp->data_queue, &event, portMAX_DELAY))
            //     sys_comp->ha.send_data(event);
        }
    };
    
    xTaskCreate(
        ha_data_rec,
        "HA Data Sender",
        2048,
        &sys_comp,
        10,
        NULL
    );
    #pragma endregion

    return ESP_OK;
}