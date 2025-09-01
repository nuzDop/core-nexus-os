#include "vmm.h"
#include "../lib/string.h"

void print(char* str);
extern void load_page_directory(uint32_t);
extern void enable_paging();

page_directory_t* kernel_directory = 0;
page_directory_t* current_directory = 0;

void vmm_init() {
    kernel_directory = (page_directory_t*)pmm_alloc_page();
    for (int i = 0; i < 1024; i++) {
        kernel_directory->entries[i] = 0x00000002;
    }
    kernel_directory->physical_addr = (uint32_t)kernel_directory;

    page_table_t* first_table = (page_table_t*)pmm_alloc_page();
    for (int i = 0; i < 1024; i++) {
        first_table->entries[i] = (i * PAGE_SIZE) | 3;
    }
    kernel_directory->entries[0] = ((uint32_t)first_table) | 3;

    current_directory = kernel_directory;
    switch_page_directory(current_directory);
    enable_paging();
    
    print("Paging enabled.\n");
}

void switch_page_directory(page_directory_t* dir) {
    current_directory = dir;
    load_page_directory(dir->physical_addr);
}

pte_t* get_page(uint32_t virt, int make, page_directory_t* dir) {
    uint32_t table_idx = virt / 1024 / PAGE_SIZE;
    if (dir->entries[table_idx]) {
        page_table_t* table = (page_table_t*)(dir->entries[table_idx] & ~0xFFF);
        return &table->entries[(virt / PAGE_SIZE) % 1024];
    } else if (make) {
        page_table_t* new_table = (page_table_t*)pmm_alloc_page();
        for(int i = 0; i < 1024; i++) new_table->entries[i] = 0;
        dir->entries[table_idx] = (uint32_t)new_table | 0x7; // Present, R/W, User
        return &new_table->entries[(virt / PAGE_SIZE) % 1024];
    }
    return 0;
}

void map_page(uint32_t virt, uint32_t phys, page_directory_t* dir) {
    pte_t* page = get_page(virt, 1, dir);
    *page = phys | 0x7; // Present, R/W, User
}

page_directory_t* clone_directory(page_directory_t* src) {
    page_directory_t* new_dir = (page_directory_t*)pmm_alloc_page();
    new_dir->physical_addr = (uint32_t)new_dir;

    for (int i = 0; i < 1024; i++) {
        if (src->entries[i] & 1) { // If the table is present in source
             // For now, kernel-space tables are linked, not copied
            if (i * 1024 * PAGE_SIZE < 0xC0000000) {
                 // User-space, needs proper copying (not implemented yet for fork)
            } else {
                new_dir->entries[i] = src->entries[i];
            }
        }
    }
    return new_dir;
}
