#include "ramdisk.h"
#include "../mem/pmm.h"
#include "../../../arch/x86_64/gdt.h" // For print function

static uint8_t* ramdisk_storage = NULL;
static size_t ramdisk_size = 0;
// We'll use a fixed block size for simplicity, matching common hardware.
#define RAMDISK_BLOCK_SIZE 512

void print(char*); // Forward declare from main.c for logging

void ramdisk_init(size_t size) {
    // Ensure size is a multiple of the page size for easy allocation
    if (size % PAGE_SIZE != 0) {
        size = (size / PAGE_SIZE + 1) * PAGE_SIZE;
    }

    ramdisk_storage = (uint8_t*)pmm_alloc_page(); // In a real scenario, we'd allocate multiple pages
    if (!ramdisk_storage) {
        print("RAMDISK Error: Failed to allocate memory for ramdisk.\n");
        return;
    }
    
    ramdisk_size = size;
    
    // Zero out the ramdisk memory
    for (size_t i = 0; i < ramdisk_size; i++) {
        ramdisk_storage[i] = 0;
    }

    print("RAM disk initialized.\n");
}

void ramdisk_read(uint32_t block_no, uint32_t count, void* buffer) {
    uint32_t offset = block_no * RAMDISK_BLOCK_SIZE;
    uint32_t len = count * RAMDISK_BLOCK_SIZE;
    
    if (offset + len > ramdisk_size) {
        // Handle error: read out of bounds
        return;
    }
    
    uint8_t* src = ramdisk_storage + offset;
    uint8_t* dest = (uint8_t*)buffer;
    for (uint32_t i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}

void ramdisk_write(uint32_t block_no, uint32_t count, void* buffer) {
    uint32_t offset = block_no * RAMDISK_BLOCK_SIZE;
    uint32_t len = count * RAMDISK_BLOCK_SIZE;

    if (offset + len > ramdisk_size) {
        // Handle error: write out of bounds
        return;
    }

    uint8_t* src = (uint8_t*)buffer;
    uint8_t* dest = ramdisk_storage + offset;
    for (uint32_t i = 0; i < len; i++) {
        dest[i] = src[i];
    }
}
