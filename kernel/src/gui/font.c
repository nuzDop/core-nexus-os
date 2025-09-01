#include "font.h"
#include "../drivers/vbe.h"

// The font data itself (a small subset for demonstration)
const uint8_t default_font[128][8] = {
    [0 ... 127] = {0}, // Initialize all to blank
    ['A'] = {0x18, 0x3C, 0x66, 0x7E, 0x66, 0x66, 0x66, 0x00},
    ['B'] = {0x7C, 0x66, 0x66, 0x7C, 0x66, 0x66, 0x7C, 0x00},
    ['C'] = {0x3C, 0x66, 0x60, 0x60, 0x60, 0x66, 0x3C, 0x00},
    ['D'] = {0x78, 0x6C, 0x66, 0x66, 0x66, 0x6C, 0x78, 0x00},
    ['e'] = {0x00, 0x00, 0x38, 0x6C, 0x70, 0x60, 0x38, 0x00},
    ['l'] = {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x38, 0x00},
    ['o'] = {0x00, 0x00, 0x38, 0x6C, 0x6C, 0x6C, 0x38, 0x00},
    // ... and so on for all other characters
};

void draw_char(char c, int x, int y, uint32_t color) {
    if ((uint8_t)c >= 128) return;

    const uint8_t* glyph = default_font[(uint8_t)c];

    for (int cy = 0; cy < 8; cy++) {
        for (int cx = 0; cx < 8; cx++) {
            if ((glyph[cy] >> (7 - cx)) & 1) {
                put_pixel(x + cx, y + cy, color);
            }
        }
    }
}

void draw_string(const char* str, int x, int y, uint32_t color) {
    int start_x = x;
    while (*str) {
        draw_char(*str, start_x, y, color);
        start_x += 8; // Move to the next character position
        str++;
    }
}
