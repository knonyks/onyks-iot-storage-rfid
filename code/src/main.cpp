#include "config.hpp"

#include "R200.hpp"
#include "WS2812B.hpp"

R200 rfid_module{};
WS2812B led{};

extern "C" void app_main(void) {

    led.init();
    ESP_LOGI(TAG_LED, "LED initialized on pin %d", LED_PIN);
    rfid_module.init();
    ESP_LOGI(TAG_R200, "R200 initialized on UART%d (TX pin %d, RX pin %d)", R200_UART, R200_TX_PIN, R200_RX_PIN);

    led.set_colour(0, 150, 0);

    while (1) {
        led.toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}