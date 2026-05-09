#include "config.hpp"

#include "R200.hpp"
#include "WS2812B.hpp"

R200 rfid_module{};
WS2812B led{};

extern "C" void app_main(void) {

    led.init();
    rfid_module.init();

    led.set_colour(0, 150, 0);

    while (1) {
        led.toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}