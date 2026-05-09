#pragma once
#include "config.hpp"

#include "led_strip.h"

class WS2812B {
    private:
        led_strip_handle_t _led;
        uint8_t r, g, b;
        bool led_state;
    public:
        WS2812B() : _led{nullptr}, r{0}, g{0}, b{0}, led_state{false} {};

        void init();
        void set_colour(uint8_t r, uint8_t g, uint8_t b);
        void toggle();
        void clear();
};