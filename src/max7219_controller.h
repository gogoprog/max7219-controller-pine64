#pragma once

#include <array>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/resource.h>
#include <unistd.h>

#include "glyphs.h"

#ifdef PINE64
#include "gpio.h"
#include "i2c.h"
#include "pinout.h"
#include "spi.h"
#endif

using byte = uint8_t;

template <int columns, int rows>
class Max7219Controller
#ifdef PINE64
    : public Pine64::GPIO
#endif
{
  private:
    int dinPin;
    int csPin;
    int clkPin;

  public:
    byte shadow[8];
    std::array<byte, 8 * columns> backBuffer;
    int width{columns * 8};
    int height{rows * 8};

    Max7219Controller(const int din, const int cs, const int clk)
#ifdef PINE64
        : GPIO()
#endif
    {
        dinPin = din;
        csPin = cs;
        clkPin = clk;
    };

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

    void regSetMax7219(byte reg, byte value) {
        if ((reg >= 1) && (reg <= 8)) {
            shadow[reg - 1] = value;
        }
        digitalWrite(csPin, LOW);
        write(reg);
        write(value);
        digitalWrite(csPin, HIGH);
    }

    void write(byte addr, byte data) {
        write(addr);
        write(data);
    }

    void clear() {
        std::fill(backBuffer.begin(), backBuffer.end(), 0);
    }

    void brightness(byte br) {
        regSetMax7219(0x0a, br);
    }

    void set(byte x, byte y) {
        if (x < 0 || y < 0 || x >= width || y >= height) {
            return;
        }

        int l = x / 8;
        byte v = 1 << (7 - (x % 8));
        backBuffer[y * columns + l] |= v;
    }

    bool setup() {

#ifdef PINE64
        if (GPIO::setup() != 0) {
            return false;
        }

        pinMode(dinPin, OUTPUT);
        pinMode(clkPin, OUTPUT);
        pinMode(csPin, OUTPUT);
        regSetMax7219(0x0b, 0x07); // Scan Limit
        regSetMax7219(0x09, 0x00); // Decode Mode
        regSetMax7219(0x0c, 0x01); // Shut Down Mode (On)
        regSetMax7219(0x0f, 0x00); // Display Test (Off)
        brightness(0);             // Brightness (0 - 15)
        for (int i = 1; i < 9; i++) {
            regSetMax7219(i, 0);
        }

#endif
        return true;
    }

    void drawGlyph(const int x, const int y, const char character) {
        for (int r = 0; r < 6; r++) {
            for (int c = 0; c < 4; c++) {
                int g = console_font_4x6[character * 6 + r];
                auto v = g >> (7 - c);
                if ((v & 1)) {
                    set(x + c, y + r);
                }
            }
        }
    }

    void drawText(const int x, const int y, const std::string text) {

        int i{0};
        for (auto c : text) {
            drawGlyph(x + 4 * i, y, c);

            ++i;
        }
    }

    void render() {
        for (int i = 0; i < height; i++) {
            digitalWrite(csPin, LOW);

            for (int c = 0; c < columns; c++) {
                write(i + 1, backBuffer[i * columns + c]);
            }
            digitalWrite(csPin, HIGH);
        }
    }
};
