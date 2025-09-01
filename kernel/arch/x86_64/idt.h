#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// An entry in the Interrupt Descriptor Table
struct idt_entry_struct {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

// The IDT pointer structure
struct idt_ptr_struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

// Public function to initialize the IDT
void init_idt();

#endif
