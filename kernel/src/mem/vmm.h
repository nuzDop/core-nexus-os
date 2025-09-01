#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include "../proc/task.h"
#include "pmm.h"

#define PAGE_SIZE 4096

// x86_64 Paging Structures
// Each table has 512 64-bit entries.
typedef uint64_t pml4_entry_t;
typedef uint64_t pdpt_entry_t;
typedef uint64_t pde_t;
typedef uint64_t pte_t;

typedef struct page_table {
    pte_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct page_directory {
    pde_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) page_directory_t;

typedef struct page_directory_pointer_table {
    pdpt_entry_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pdpt_t;

typedef struct page_map_level_4 {
    pml4_entry_t entries[512];
} __attribute__((aligned(PAGE_SIZE))) pml4_t;


// Public functions
void vmm_init();
void vmm_map_page(pml4_t* pml4, uint64_t virt_addr, uint64_t phys_addr);

#endif
