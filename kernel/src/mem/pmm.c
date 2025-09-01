#include "pmm.h"

// For simplicity, we assume a maximum of 128MB of RAM for the bitmap.
// A real kernel would get the memory map from the bootloader.
#define MAX_MEMORY_MB 128
#define BITMAP_SIZE (MAX_MEMORY_MB * 1024 * 1024 / PAGE_SIZE / 8)

static uint8_t memory_bitmap[BITMAP_SIZE];
static size_t total_pages = 0;
static size_t last_checked_page = 0;

// Sets a bit in the bitmap (marks a page as used)
static void set_page_used(size_t page_index) {
    memory_bitmap[page_index / 8] |= (1 << (page_index % 8));
}

// Clears a bit in the bitmap (marks a page as free)
static void set_page_free(size_t page_index) {
    memory_bitmap[page_index / 8] &= ~(1 << (page_index % 8));
}

// Checks if a bit is set (if a page is used)
static int is_page_used(size_t page_index) {
    return (memory_bitmap[page_index / 8] & (1 << (page_index % 8))) != 0;
}

// Initialize the PMM
void pmm_init(size_t total_memory) {
    total_pages = total_memory / PAGE_SIZE;

    // Mark all pages as used initially
    for (size_t i = 0; i < BITMAP_SIZE; ++i) {
        memory_bitmap[i] = 0xFF;
    }

    // For now, let's assume the first 16MB are available for use.
    // A real implementation would parse a memory map from GRUB.
    for (size_t i = 0; i < (16 * 1024 * 1024 / PAGE_SIZE); ++i) {
        set_page_free(i);
    }

    // Mark the first page (used by BIOS/GRUB) as used
    set_page_used(0);
}

// Find the first free page and allocate it
void* pmm_alloc_page() {
    for (size_t i = last_checked_page; i < total_pages; ++i) {
        if (!is_page_used(i)) {
            set_page_used(i);
            last_checked_page = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }
    
    // If we didn't find one, wrap around and check from the beginning
    for (size_t i = 0; i < last_checked_page; ++i) {
        if (!is_page_used(i)) {
            set_page_used(i);
            last_checked_page = i + 1;
            return (void*)(i * PAGE_SIZE);
        }
    }

    return NULL; // Out of memory
}

// Free a previously allocated page
void pmm_free_page(void* p) {
    size_t page_index = (size_t)p / PAGE_SIZE;
    if (page_index < total_pages) {
        set_page_free(page_index);
        if (page_index < last_checked_page) {
            last_checked_page = page_index;
        }
    }
}
