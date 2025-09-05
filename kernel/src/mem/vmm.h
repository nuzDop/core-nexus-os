#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stdbool.h>
#include "../proc/task.h"
#include "pmm.h"

#define PAGE_SIZE 4096

// x86 Paging Structures
typedef struct page_table_entry {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} __attribute__((packed)) page_table_entry_t;

typedef struct page_directory_entry {
    uint32_t present    : 1;
    uint32_t rw         : 1;
    uint32_t user       : 1;
    uint32_t accessed   : 1;
    uint32_t dirty      : 1;
    uint32_t unused     : 7;
    uint32_t frame      : 20;
} __attribute__((packed)) page_directory_entry_t;


typedef struct page_table {
    page_table_entry_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_directory_entry_t tables[1024];
    uint32_t tablesPhysical[1024];
    uint32_t physical_addr;
} page_directory_t;

void vmm_init();
void map_page(uint32_t virt, uint32_t phys, page_directory_t* dir);
page_directory_t* clone_directory(page_directory_t* src, bool cow);
void switch_page_directory(page_directory_t* dir);

extern page_directory_t* kernel_directory;

#endif
