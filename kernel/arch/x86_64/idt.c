#include "idt.h"

idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

extern void isr0();
extern void irq0(); // Timer
extern void irq1(); // Keyboard
extern void irq12(); // Mouse
extern void isr128();

extern void idt_flush(uint32_t);

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;
    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags | 0x60;
}

void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (uint32_t)&idt_entries;

    unsigned char* idt_ptr_byte = (unsigned char*)&idt_entries;
    for(int i=0; i < sizeof(idt_entry_t) * 256; i++) {
        idt_ptr_byte[i] = 0;
    }

    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);   // IRQ0: Timer
    idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);  // IRQ1: PS/2 Keyboard
    idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);  // IRQ12: PS/2 Mouse
    idt_set_gate(128, (uint32_t)isr128, 0x08, 0xEE);

    idt_flush((uint32_t)&idt_ptr);
}
