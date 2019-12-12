# max7219-controller-pine64

## Features

 * World space pixel positions
 * Draw text using predefined fonts
 * Draw lines
 * Draw rectangles
 * Simulator using SDL2

## Demo

![Image](https://gogoprog.github.io/max7219-controller-pine64/video.gif)

![Image](https://gogoprog.github.io/max7219-controller-pine64/video2.gif)

## Example

See `src/example/main.cpp` :

```cpp
    auto *controller = new Max7219Controller<4, 1>(
      Pine64::PI_GPIO_14,
      Pine64::PI_GPIO_15,
      Pine64::PI_GPIO_18
      );
    int x = 0;
    while (true) {
        controller->clear();
        controller->drawText(x, 0, "... MAX7219 Controller for the Pine64", Font::font5x8);
        controller->render();
        controller->delay(100);
        x--;
    }
```
