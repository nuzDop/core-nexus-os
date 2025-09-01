#include "vbe.h"

static uint32_t* framebuffer;
static uint16_t screen_width;
static uint16_t screen_height;
static uint16_t screen_pitch;

void print(char*); // Forward declare

// In a real OS, this function would make a VBE BIOS interrupt (int 0x10)
// to find a suitable mode and get its info. We will simulate this by
// hardcoding values for a 1024x768 32bpp mode.
uint32_t* vbe_init(uint16_t width, uint16_t height, uint16_t bpp) {
    // These values would normally be returned by the VBE info block.
    // We assume a common framebuffer address for emulators like QEMU.
    vbe_mode_info_t vbe_info;
    vbe_info.physbase = 0xE0000000; // A common LFB address
    vbe_info.Xres = width;
    vbe_info.Yres = height;
    vbe_info.pitch = width * (bpp / 8);

    framebuffer = (uint32_t*)vbe_info.physbase;
    screen_width = vbe_info.Xres;
    screen_height = vbe_info.Yres;
    screen_pitch = vbe_info.pitch;

    // A real implementation would need to map this physical memory
    // into the kernel's virtual address space. We assume it's already accessible.
    
    print("VBE: Graphics mode initialized.\n");
    return framebuffer;
}

void put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return;
    }
    // The pitch is in bytes, but our framebuffer is a uint32_t*, so we divide by 4.
    unsigned int where = x + y * (screen_pitch / 4);
    framebuffer[where] = color;
}

// Helper to draw a filled rectangle
void fill_rect(int x, int y, int w, int h, uint32_t color) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            put_pixel(j, i, color);
        }
    }
}
