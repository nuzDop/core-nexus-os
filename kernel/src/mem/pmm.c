#include "pmm.h"
#include "../boot/multiboot.h"

#define MAX_MEMORY_MB 128
#define BITMAP_SIZE (MAX_MEMORY_MB * 1024 * 1024 / PAGE_SIZE / 8)

static uint8_t memory_bitmap[BITMAP_SIZE];
static size_t total_pages = 0;
static size_t last_checked_page = 0;

void print(char*); // Forward declare

static void set_page_used(size_t page_index) {
    if (page_index >= total_pages) return;
    memory_bitmap[page_index / 8] |= (1 << (page_index % 8));
}

static void set_page_free(size_t page_index) {
    if (page_index >= total_pages) return;
    memory_bitmap[page_index / 8] &= ~(1 << (page_index % 8));
}

static int is_page_used(size_t page_index) {
    if (page_index >= total_pages) return 1; // Out of bounds is "used"
    return (memory_bitmap[page_index / 8] & (1 << (page_index % 8))) != 0;
}

void pmm_init(multiboot_info_t* mbd) {
    // 1. Calculate total memory to determine bitmap size
    uint32_t mem_size_kb = mbd->mem_lower + mbd->mem_upper;
    total_pages = (mem_size_kb * 1024) / PAGE_SIZE;

    // 2. Mark all memory as used by default
    for (size_t i = 0; i < BITMAP_SIZE; ++i) {
        memory_bitmap[i] = 0xFF;
    }

    // 3. Iterate through the memory map and mark available pages as free
    mmap_entry_t* mmap = (mmap_entry_t*)mbd->mmap_addr;
    while ((uint32_t)mmap < mbd->mmap_addr + mbd->mmap_length) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            // Mark all pages in this available region as free
            for (uint64_t i = 0; i < mmap->len; i += PAGE_SIZE) {
                set_page_free((mmap->addr + i) / PAGE_SIZE);
            }
        }
        mmap = (mmap_entry_t*)((uint32_t)mmap + mmap->size + sizeof(uint32_t));
    }

    // Mark the first page (used by BIOS/GRUB/kernel itself) as used
    set_page_used(0);
}

void* pmm_alloc_page() {
    for (size_t i = last_checked_page; i < total_pages; ++i) {
        if (!is_page_used(i)) {
            set_page_used(i);
            last_checked_page = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }
    for (size_t i = 0; i < last_checked_page; ++i) {
        if (!is_page_used(i)) {
            set_page_used(i);
            last_checked_page = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

void pmm_free_page(void* p) {
    size_t page_index = (size_t)p / PAGE_SIZE;
    if (page_index < total_pages) {
        set_page_free(page_index);
        if (page_index < last_checked_page) {
            last_checked_page = page_index;
        }
    }
}
