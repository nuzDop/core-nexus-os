#include "arch/x86_64/gdt.h"
#include "lib/string.h" // Corrected include path

// Define the GDT entries and pointer
static gdt_entry_t gdt_entries[5];
static gdt_ptr_t   gdt_ptr;

// External assembly function to load the GDT register
extern void gdt_flush(uint64_t);

// Setup a GDT descriptor
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Initialize the Global Descriptor Table
void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    // Use uint64_t for 64-bit addresses
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0, 0x9A, 0x20);          // Kernel Code Segment (64-bit)
    gdt_set_gate(2, 0, 0, 0x92, 0x00);          // Kernel Data Segment
    gdt_set_gate(3, 0, 0, 0xFA, 0x20);          // User Code Segment
    gdt_set_gate(4, 0, 0, 0xF2, 0x00);          // User Data Segment

    gdt_flush((uint64_t)&gdt_ptr);
}
