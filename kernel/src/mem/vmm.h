#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include "../proc/task.h"
#include "pmm.h"

#define PAGE_SIZE 4096

typedef uint32_t pte_t;
typedef uint32_t pde_t;

typedef struct page_table {
    pte_t entries[1024];
} page_table_t;

typedef struct page_directory {
    pde_t entries[1024];
    uint32_t physical_addr;
} page_directory_t;

// Public functions
void vmm_init();
page_directory_t* clone_directory(page_directory_t* src);
void switch_page_directory(page_directory_t* dir);

// Page manipulation functions
void map_page(uint32_t virt, uint32_t phys, page_directory_t* dir);
pte_t* get_page(uint32_t virt, int make, page_directory_t* dir);

extern page_directory_t* kernel_directory;
extern page_directory_t* current_directory;

#endif
