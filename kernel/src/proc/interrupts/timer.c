#include "timer.h"
#include "../proc/task.h"

uint32_t tick = 0;

// The handler for the timer interrupt.
static void timer_callback() {
    tick++;
    // Switch task on every timer tick. This drives the multitasking.
    switch_task();
}

// We need a way to register this callback with the interrupt handler.
// For now, this is a placeholder for a more complex interrupt registration system.
void register_timer_callback(void (*callback)());
// In interrupts.asm, we will call a generic C handler, which will then call this.
void timer_handler_c() {
    timer_callback();
}


void init_timer(uint32_t frequency) {
    // Register our callback
    register_timer_callback(timer_callback);

    // The PIT operates at 1.193182 MHz
    uint32_t divisor = 1193182 / frequency;

    // Send the command byte
    // Port 0x43 is the command port
    // 0x36 sets the PIT to repeating mode
    // outb(0x43, 0x36);

    // Send the frequency divisor
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
    
    // outb(0x40, l);
    // outb(0x40, h);
    // NOTE: outb (out byte) is a function we'd need to write in assembly.
    // For now, the logic is here, but it won't actually configure the hardware.
}
