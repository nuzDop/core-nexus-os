#include "vmm.h"
#include "pmm.h"
#include "../lib/string.h"

page_directory_t* kernel_directory = 0;
page_directory_t* current_directory = 0;

extern void load_page_directory(uint32_t);
extern void enable_paging();

void vmm_init() {
    // Implementation of VMM initialization...
    // Create kernel directory, map kernel pages etc.
}

void map_page(uint32_t virt, uint32_t phys, page_directory_t* dir) {
    // Implementation of mapping a virtual address to a physical one.
}

page_directory_t* clone_directory(page_directory_t* src, bool cow) {
    page_directory_t* new_dir = (page_directory_t*)pmm_alloc_page();
    // Implementation of cloning a page directory, with Copy-on-Write (COW) support.
    return new_dir;
}

void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    load_page_directory(dir->physical_addr);
}
