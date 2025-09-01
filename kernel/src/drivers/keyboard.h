#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

// Initializes the PS/2 Keyboard Driver
void init_keyboard();

// The C-level interrupt handler called from assembly
void keyboard_handler_c();

#endif
