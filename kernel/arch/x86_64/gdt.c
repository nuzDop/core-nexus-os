#include "gdt.h"
#include "../../lib/string.h"

// Define the GDT entries
static gdt_entry_t gdt_entries[5];
static gdt_ptr_t   gdt_ptr;

// Function to set a GDT entry
static void gdt_set_gate(int32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Initialize the GDT
void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    // Pointers are now 64-bit. Cast to uint64_t.
    gdt_ptr.base  = (uint64_t)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Kernel Code Segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Kernel Data Segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User Code Segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User Data Segment

    // Load the GDT
    // The assembly function now expects a 64-bit pointer.
    gdt_flush((uint64_t)&gdt_ptr);
}
