#include "WS2812B.hpp"

void WS2812B::init() {
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_PIN,
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

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &_led));
    ESP_LOGI(TAG_LED, "LED initialized on pin %d", LED_PIN);
}

void WS2812B::set_colour(uint8_t r, uint8_t g, uint8_t b) {
    this->r = r;
    this->g = g;
    this->b = b;
}

void WS2812B::toggle() {
    led_state = !led_state;

    if (led_state) {
        led_strip_set_pixel(_led, 0, r, g, b);
    } else {
        led_strip_clear(_led);
    }
    
    led_strip_refresh(_led);
}

void WS2812B::clear() {
    led_strip_clear(_led);
    led_strip_refresh(_led);
}