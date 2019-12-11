#include "max7219_controller.h"
#include <iostream>

int main(void) {
    auto *controller = new Max7219Controller<4, 1>(Pine64::PI_GPIO_14, Pine64::PI_GPIO_15, Pine64::PI_GPIO_18);

    if(!controller->setup()) {
        return 1;
    }

    int x = 0;

    while (true) {

        controller->clear();

        controller->set(x, 1);

        controller->drawText(x, 2, "Gogoprog!");
        x--;

        controller->render();

        controller->delay(500);
    }

    return 0;
}
