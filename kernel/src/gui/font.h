#ifndef FONT_H
#define FONT_H

#include <stdint.h>

// Represents a single 8x8 character glyph
// Each byte represents a row of 8 pixels.
extern const uint8_t default_font[128][8];

// Function to draw a character to the screen
void draw_char(char c, int x, int y, uint32_t color);

// Function to draw a string to the screen
void draw_string(const char* str, int x, int y, uint32_t color);

#endif
