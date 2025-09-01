#ifndef VBE_H
#define VBE_H

#include <stdint.h>

// VBE Mode Information Block Structure
typedef struct {
    uint16_t attributes;
    uint8_t  winA, winB;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segmentA, segmentB;
    uint32_t realFctPtr;
    uint16_t pitch; // Bytes per scanline

    uint16_t Xres, Yres;
    uint8_t  Wchar, Ychar, planes, bpp, banks;
    uint8_t  memory_model, bank_size, image_pages;
    uint8_t  reserved0;

    uint8_t  red_mask, red_position;
    uint8_t  green_mask, green_position;
    uint8_t  blue_mask, blue_position;
    uint8_t  rsv_mask, rsv_position;
    uint8_t  directcolor_attributes;

    uint32_t physbase; // Physical address of the linear framebuffer
    uint32_t reserved1;
    uint16_t reserved2;
} __attribute__((packed)) vbe_mode_info_t;

// Initializes the VBE driver and sets a graphics mode.
// Returns a pointer to the linear framebuffer.
uint32_t* vbe_init(uint16_t width, uint16_t height, uint16_t bpp);

// Puts a single pixel on the screen at the given coordinates.
void put_pixel(int x, int y, uint32_t color);

#endif
