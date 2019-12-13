#include "../max7219/controller.h"
#include "luabinder.hpp"

int main(void) {
    rf::wrapper::LuaBinder lua;

    lua.dofile("main.lua");

        lua.call_function<void>("load");
    auto *controller = new max7219::Controller(4, 1, Pine64::PI_GPIO_14, Pine64::PI_GPIO_15, Pine64::PI_GPIO_18);

    if (!controller->setup()) {
        return 1;
    }

    int x = 0;

    while (true) {
        lua.call_function<void>("update", x);

        controller->clear();

        controller->set(x, 1);

        controller->drawText(x, 0, "GAME FRAMEWORK", max7219::Font::font5x8);
        x--;

        controller->render();

        controller->delay(100);
    }

    return 0;
}
