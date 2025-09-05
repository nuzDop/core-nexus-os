#include "keyboard.h"
#include "../gui/compositor.h"
#include "../proc/task.h"

// Helper function to read from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// US QWERTY scancode map
const char scancode_to_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '?', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '
};

// The C-level keyboard interrupt handler
void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);

    if (scancode < 0x80) { // Key press
        char ascii = '?';
        if (scancode < sizeof(scancode_to_ascii)) {
            ascii = scancode_to_ascii[scancode];
        }

        window_t* active_win = compositor_get_active_window();
        if (active_win && active_win->owner_pid > 0) {
            task_t* owner = find_task_by_id(active_win->owner_pid);
            if (owner) {
                event_t event = { .type = EVENT_KEY_PRESS, .data1 = (int32_t)ascii, .data3 = active_win->id };
                push_event_to_task(owner, event);
            }
        }
    }
}

void init_keyboard() {
    // A full implementation would properly configure the PS/2 controller.
    print("Keyboard driver initialized.\n");
}
