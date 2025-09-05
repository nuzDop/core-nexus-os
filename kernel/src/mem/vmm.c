/* kernel/src/mem/vmm.c */

#include "vmm.h"
#include "pmm.h"
#include "../lib/string.h"
#include <stdbool.h>

// The kernel's page directory.
page_directory_t* kernel_directory = 0;

// The current page directory.
volatile page_directory_t* current_directory = 0;

// External assembly functions to load page directory and enable paging.
extern void load_page_directory(uint32_t*);
extern void enable_paging();

void vmm_init() {
    // Allocate a page for the kernel page directory, ensuring it's page-aligned.
    kernel_directory = (page_directory_t*)pmm_alloc_page();
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physical_addr = (uint32_t)kernel_directory;

    // Identity map the first 4MB of memory. This is crucial so the kernel
    // can continue executing from its physical address after paging is enabled.
    for (uint32_t i = 0; i < 0x400000; i += PAGE_SIZE) {
        map_page(i, i, kernel_directory);
    }

    // A real implementation would also map the kernel to a higher-half address (e.g., 0xC0000000).

    // Switch to our new page directory.
    switch_page_directory(kernel_directory);
    enable_paging();
}

void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    load_page_directory((uint32_t*)&dir->tablesPhysical);
}

void map_page(uint32_t virt, uint32_t phys, page_directory_t* dir) {
    uint32_t dir_index = virt / (1024 * PAGE_SIZE);
    uint32_t table_index = (virt / PAGE_SIZE) % 1024;

    // If the page table for this address doesn't exist, create it.
    if (!dir->tables[dir_index]) {
        // Allocate a page for the new page table.
        page_table_t* new_table = (page_table_t*)pmm_alloc_page();
        memset(new_table, 0, sizeof(page_table_t));
        
        // Update the directory with the new table's physical address and flags.
        dir->tables[dir_index] = new_table;
        dir->tablesPhysical[dir_index] = (uint32_t)new_table | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
    }

    // Get the page table.
    page_table_t* table = dir->tables[dir_index];
    
    // Map the virtual address to the physical frame with appropriate flags.
    table->pages[table_index] = (phys & 0xFFFFF000) | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
}

// Clones a page directory. Used by fork().
page_directory_t* clone_directory(page_directory_t* src, bool cow) {
    // Allocate a new page directory.
    page_directory_t* new_dir = (page_directory_t*)pmm_alloc_page();
    memset(new_dir, 0, sizeof(page_directory_t));
    new_dir->physical_addr = (uint32_t)new_dir;

    for (int i = 0; i < 1024; i++) {
        if (!src->tables[i]) continue;
        
        // If the entry points to a kernel page table, link it directly.
        if ((uint32_t)src->tables[i] < 0x400000) { // Simple check for kernel space
            new_dir->tables[i] = src->tables[i];
            new_dir->tablesPhysical[i] = src->tablesPhysical[i];
        } else {
            // This is a user-space page table. Apply Copy-on-Write.
            if (cow) {
                // Point the new directory to the same page table.
                new_dir->tables[i] = src->tables[i];
                new_dir->tablesPhysical[i] = src->tablesPhysical[i];
                
                // Mark both as read-only. The hardware will trigger a page fault
                // on write, which the fault handler will use to perform the actual copy.
                new_dir->tablesPhysical[i] &= ~PTE_WRITABLE;
                src->tablesPhysical[i] &= ~PTE_WRITABLE;
            } else {
                // Full copy (not COW).
                page_table_t* new_table = (page_table_t*)pmm_alloc_page();
                memcpy(new_table, src->tables[i], sizeof(page_table_t));
                new_dir->tables[i] = new_table;
                new_dir->tablesPhysical[i] = (uint32_t)new_table | PTE_PRESENT | PTE_WRITABLE | PTE_USER;
            }
        }
    }
    
    return new_dir;
}
