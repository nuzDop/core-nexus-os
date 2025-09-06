#include "idt.h"
#include "../../lib/string.h"

// Define the IDT and IDT pointer
static idt_entry_t idt_entries[256];
static idt_ptr_t   idt_ptr;

// External ISR/IRQ stubs from interrupts.asm
extern void isr0();
extern void irq0();
extern void irq1();
extern void irq12();
extern void isr128();


// Set an IDT entry
static void idt_set_gate(uint8_t num, uint64_t base, uint16_t sel, uint8_t flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_mid = (base >> 16) & 0xFFFF;
    idt_entries[num].base_hi = (base >> 32) & 0xFFFFFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}

// Initialize the IDT
void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    // Pointers are now 64-bit.
    idt_ptr.base  = (uint64_t)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t) * 256);

    // Set up ISRs and IRQs. The base address must be a 64-bit integer.
    idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
    idt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);    // IRQ0: Timer
    idt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);   // IRQ1: PS/2 Keyboard
    idt_set_gate(44, (uint64_t)irq12, 0x08, 0x8E);  // IRQ12: PS/2 Mouse
    idt_set_gate(128, (uint64_t)isr128, 0x08, 0xEE); // Syscall vector

    // Load the IDT
    idt_flush((uint64_t)&idt_ptr);
}
