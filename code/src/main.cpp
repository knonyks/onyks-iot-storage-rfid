#include "main.hpp"

static const char *TAG_LED = "WS2812B";

extern "C" void app_main(void) {

    led_strip_handle_t led;
    bool led_on = false;

    led_init(&led);

    while (1) {
        if (!led_on) {
            // ustawianie koloru (uchwyt, indeks, R, G, B)
            led_strip_set_pixel(led, 0, 0, 150, 0); 
            led_strip_refresh(led);
            ESP_LOGI(TAG_LED, "LED ON");
        } else {
            led_strip_clear(led);
            led_strip_refresh(led);
            ESP_LOGI(TAG_LED, "LED OFF");
        }

        led_on = !led_on;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void led_init(led_strip_handle_t *led) {
    
    led_strip_config_t strip_config = {
        .strip_gpio_num = GPIO_LED,
        .max_leds = 1,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB,
        .led_model = LED_MODEL_WS2812,
        .flags = { .invert_out = false }
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz wymagane dla diody WS2812B
        .mem_block_symbols = 64, // bufor na bity z danymi (kolorami)
        .flags = { .with_dma = false }
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, led));
    ESP_LOGI(TAG_LED, "led initialised on GPIO%d", GPIO_LED);
}