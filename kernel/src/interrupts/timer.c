#include "timer.h"
#include "../proc/task.h"

uint32_t tick = 0;

static void timer_callback(registers_t* regs) {
    (void)regs;
    tick++;
    switch_task();
}

void init_timer(uint32_t frequency) {
    register_interrupt_handler(IRQ0, timer_callback);

    uint32_t divisor = 1193182 / frequency;
    outb(0x43, 0x36);
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);
    outb(0x40, l);
    outb(0x40, h);
}
