#include "../max7219/controller.h"
#include <iostream>

int main(void) {
    auto *controller = new max7219::Controller<4, 1>(Pine64::PI_GPIO_14, Pine64::PI_GPIO_15, Pine64::PI_GPIO_18);

    if(!controller->setup()) {
        return 1;
    }

    int x = 0;

    while (true) {

        controller->clear();

        controller->set(x, 1);

        controller->drawText(x, 0, "... MAX7219 Controller for the Pine64", max7219::Font::font5x8);
        x--;

        controller->render();

        controller->delay(100);
    }

    return 0;
}
