#ifndef PMM_H
#define PMM_H

#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096 // Define our page size as 4KB

// Initializes the physical memory manager.
// Needs the total memory size to set up the bitmap.
void pmm_init(size_t total_memory);

// Allocates a single page of physical memory.
// Returns the physical address of the allocated page, or 0 if no pages are free.
void* pmm_alloc_page();

// Frees a single page of physical memory.
void pmm_free_page(void* p);

#endif
