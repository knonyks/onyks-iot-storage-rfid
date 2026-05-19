#include "App.hpp"

extern "C" void app_main(void) {
    App app{};

    app.init();

    app.exec();
}