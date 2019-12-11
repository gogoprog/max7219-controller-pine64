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

using byte = uint8_t;

enum class Font { font4x6, font5x8 };

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
    std::map<Font, FontData> fontDataMap;
    std::array<byte, 8 * columns> backBuffer;
    int width{columns * 8};
    int height{rows * 8};

  public:
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

    void clear() {
        std::fill(backBuffer.begin(), backBuffer.end(), 0);
    }

    void brightness(byte value) {
        setReg(0x0a, value);
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
        setReg(0x0b, 0x07); // Scan Limit
        setReg(0x09, 0x00); // Decode Mode
        setReg(0x0c, 0x01); // Shut Down Mode (On)
        setReg(0x0f, 0x00); // Display Test (Off)
        brightness(0);             // Brightness (0 - 15)
        for (int i = 1; i < 9; i++) {
            setReg(i, 0);
        }
#endif

        fontDataMap[Font::font5x8] = FontData{5, 8, font_5x8};

        return true;
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
        for (int i = 0; i < height; i++) {
            digitalWrite(csPin, LOW);

            for (int c = 0; c < columns; c++) {
                write(i + 1, backBuffer[i * columns + c]);
            }
            digitalWrite(csPin, HIGH);
        }
    }
};
