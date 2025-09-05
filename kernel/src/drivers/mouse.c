#include "mouse.h"
#include "../gui/compositor.h"
#include "../gui/window.h"
#include <stdbool.h>

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

int mouse_x = 512;
int mouse_y = 384;
static uint8_t mouse_cycle = 0;
static int8_t mouse_byte[3];
static bool mouse_left_button_down = false;

void mouse_handler_c() {
    switch(mouse_cycle) {
        case 0:
            mouse_byte[0] = inb(0x60);
            // Check for bit 3 (always 1) to ensure alignment
            if (mouse_byte[0] & 0x08) {
                mouse_cycle++;
            }
            break;
        case 1:
            mouse_byte[1] = inb(0x60);
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = inb(0x60);
            
            bool left_down = mouse_byte[0] & 0x1;
            bool left_press = left_down && !last_left_button_state;
            bool left_release = !left_down && last_left_button_state;
            last_left_button_state = left_down;

            // Update coordinates
            mouse_x += mouse_byte[1];
            mouse_y -= mouse_byte[2];
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > 1024 - 10) mouse_x = 1024 - 10;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > 768 - 10) mouse_y = 768 - 10;

            compositor_handle_mouse(mouse_x, mouse_y, left_press, left_release);
            mouse_cycle = 0;
            break;
    }
}

void mouse_wait(uint8_t a_type) {
    uint32_t timeout = 100000;
    if (a_type == 0) {
        while (timeout--) { if ((inb(0x64) & 2) == 0) return; }
    } else {
        while (timeout--) { if (inb(0x64) & 1) return; }
    }
}

void init_mouse() {
    mouse_wait(0);
    outb(0x64, 0xA8);
    mouse_wait(0);
    outb(0x64, 0x20);
    mouse_wait(1);
    uint8_t status = inb(0x60);
    status |= 2;
    mouse_wait(0);
    outb(0x64, 0x60);
    mouse_wait(0);
    outb(0x60, status);
    mouse_wait(0);
    outb(0x64, 0xD4);
    mouse_wait(0);
    outb(0x60, 0xF4);
    mouse_wait(1);
    inb(0x60);
}
