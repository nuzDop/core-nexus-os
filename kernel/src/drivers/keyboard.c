#include "keyboard.h"
#include "../gui/window.h"
#include "../proc/task.h"

// Helper function to read from an I/O port
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// US QWERTY scancode map for key presses (scancodes 0x01-0x3A)
const char scancode_to_ascii[] = {
    '?', '?', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', '?', 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', '?', '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', '?', '?', '?', ' '
};

// Externally defined functions and variables
extern window_t* get_active_window();
extern task_t* find_task_by_id(int id);
extern void push_event_to_task(task_t* task, event_t event);
void print(char*);

// The C-level keyboard interrupt handler
void keyboard_handler_c() {
    uint8_t scancode = inb(0x60);

    // We only handle key presses for now (scancodes < 0x80)
    if (scancode < 0x80) {
        char ascii = '?';
        if (scancode < sizeof(scancode_to_ascii)) {
            ascii = scancode_to_ascii[scancode];
        }

        // Find the active window and its owner process
        window_t* active_window = get_active_window();
        if (active_window) {
            task_t* owner = find_task_by_id(active_window->owner_pid);
            if (owner) {
                event_t event = { .type = EVENT_KEY_PRESS, .data1 = (int32_t)ascii };
                push_event_to_task(owner, event);
            }
        }
    }
}

void init_keyboard() {
    // A full implementation would wait for the controller and send commands
    // to enable interrupts, but for now, we just need the handler registered.
    print("Keyboard driver initialized.\n");
}
