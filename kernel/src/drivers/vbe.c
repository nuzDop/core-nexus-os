#include "vbe.h"

static uint32_t* framebuffer;
static uint16_t screen_width;
static uint16_t screen_height;
static uint16_t screen_pitch;

void print(char*); // Forward declare

uint32_t* vbe_init(uint16_t width, uint16_t height, uint16_t bpp) {
    vbe_mode_info_t vbe_info;
    vbe_info.physbase = 0xE0000000; // Common LFB address for emulators
    vbe_info.Xres = width;
    vbe_info.Yres = height;
    vbe_info.pitch = width * (bpp / 8);

    framebuffer = (uint32_t*)vbe_info.physbase;
    screen_width = vbe_info.Xres;
    screen_height = vbe_info.Yres;
    screen_pitch = vbe_info.pitch;
    
    print("VBE: Graphics mode initialized.\n");
    return framebuffer;
}

uint32_t vbe_get_width() { return screen_width; }
uint32_t vbe_get_height() { return screen_height; }
uint32_t vbe_get_pitch() { return screen_pitch; }
uint32_t* vbe_get_framebuffer() { return framebuffer; }
