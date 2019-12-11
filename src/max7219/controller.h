#pragma once

#include <array>
#include <iostream>
#include <map>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/resource.h>
#include <unistd.h>

#include "fonts.h"

#ifdef PINE64
#include "gpio.h"
#include "i2c.h"
#include "pinout.h"
#include "spi.h"
#endif

#ifdef SIMULATOR
#include "pinout.h"
#include <SDL2/SDL.h>
#include <iostream>
#endif

namespace max7219 {

using byte = uint8_t;

enum class Font { font4x6, font5x8 };

template <int columns, int rows>
class Controller
#ifdef PINE64
    : public Pine64::GPIO
#endif
{

  public:
    Controller(const int din, const int cs, const int clk)
#ifdef PINE64
        : GPIO()
#endif
    {
        dinPin = din;
        csPin = cs;
        clkPin = clk;
    }

    void clear() {
        std::fill(backBuffer.begin(), backBuffer.end(), 0);
    }

    void brightness(byte value) {
#ifdef PINE64
        setReg(0x0a, value);
#endif
    }

    bool setup() {

#ifdef PINE64
        if (GPIO::setup() != 0) {
            return false;
        }

        pinMode(dinPin, OUTPUT);
        pinMode(clkPin, OUTPUT);
        pinMode(csPin, OUTPUT);
        setReg(0x0b, 0x07); // Scan Limit
        setReg(0x09, 0x00); // Decode Mode
        setReg(0x0c, 0x01); // Shut Down Mode (On)
        setReg(0x0f, 0x00); // Display Test (Off)
        brightness(0);      // Brightness (0 - 15)
        for (int i = 1; i < 9; i++) {
            setReg(i, 0);
        }
#endif

#ifdef SIMULATOR
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            std::cout << "Failed to init SDL : " << SDL_GetError();
            return false;
        }

        window = SDL_CreateWindow("MAX7912 Simulator", 50, 50, width * 16, height * 16, 0);

        if (window == nullptr) {
            std::cout << "Failed to apply video mode : " << SDL_GetError();
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (renderer == nullptr) {
            std::cout << "Could not create renderer!";
            return false;
        }

        SDL_RenderSetLogicalSize(renderer, width, height);
#endif

        fontDataMap[Font::font5x8] = FontData{5, 8, font_5x8};
        fontDataMap[Font::font4x6] = FontData{4, 6, font_4x6};

        return true;
    }

    bool isSet(const int x, const int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return false;
        }

        int l = x / 8;
        byte v = backBuffer[y * columns + l];
        v >>= (7 - (x % 8));

        return (v & 1) == 1;
    }

    void set(const int x, const int y) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        int l = x / 8;
        byte v = 1 << (7 - (x % 8));
        backBuffer[y * columns + l] |= v;
    }

    void drawGlyph(const int x, const int y, const char character, const Font font) {
        const auto &fontData = fontDataMap.at(font);
        for (int r = 0; r < fontData.charHeight; r++) {
            for (int c = 0; c < fontData.charWidth; c++) {
                int g = fontData.data[character * fontData.charHeight + r];
                auto v = g >> (7 - c);
                if ((v & 1)) {
                    set(x + c, y + r);
                }
            }
        }
    }

    void drawText(const int x, const int y, const std::string text, const Font font) {
        const auto &fontData = fontDataMap.at(font);
        int i{0};
        for (auto c : text) {
            drawGlyph(x + fontData.charWidth * i, y, c, font);

            ++i;
        }
    }

    void render() {
#ifdef PINE64
        for (int i = 0; i < height; i++) {
            digitalWrite(csPin, LOW);

            for (int c = 0; c < columns; c++) {
                write(i + 1, backBuffer[i * columns + c]);
            }
            digitalWrite(csPin, HIGH);
        }
#endif

#ifdef SIMULATOR

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 20, 20, 255);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                if (isSet(x, y)) {
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Event e;

        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }
        }
#endif
    }

#if defined(SIMULATOR) && !defined(PINE64)
    void delay(const int ms) {
        SDL_Delay(ms);
    }

#endif

  private:
#ifdef PINE64
    void write(byte data) {
        for (byte i = 7; i < 9; i--) {
            byte mask = 1 << i;
            byte dat = data & mask;

            digitalWrite(clkPin, LOW);
            if (dat > 0) {
                digitalWrite(dinPin, HIGH);
            } else {
                digitalWrite(dinPin, LOW);
            }
            digitalWrite(clkPin, HIGH);
        }
    }

    void setReg(byte reg, byte value) {
        digitalWrite(csPin, LOW);
        write(reg);
        write(value);
        digitalWrite(csPin, HIGH);
    }

    void write(byte addr, byte data) {
        write(addr);
        write(data);
    }
#endif

    int dinPin{-1};
    int csPin{-1};
    int clkPin{-1};
    std::map<Font, FontData> fontDataMap;
    std::array<byte, 8 * columns> backBuffer;
    int width{columns * 8};
    int height{rows * 8};

#ifdef SIMULATOR
    SDL_Renderer *renderer;
    SDL_Window *window;
#endif
};
} // namespace max7219
