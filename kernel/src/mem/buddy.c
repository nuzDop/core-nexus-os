/* kernel/src/mem/buddy.c */

#include "buddy.h"
#include "../lib/string.h" // For memset
#include "pmm.h" // For PAGE_SIZE, though it could be more generic

// A node in a free list. Placed at the beginning of each free block.
typedef struct free_node {
    struct free_node* next;
} free_node_t;

// The main allocator structure
static struct {
    free_node_t* free_list[MAX_ORDER + 1];
    uintptr_t start_addr;
    uint32_t total_pages;
    uint8_t* page_orders; // An array to store the order of each allocated page
} buddy_allocator;

// Helper to get the order for a given size
static inline int get_order(size_t size) {
    int order = 0;
    size_t block_size = PAGE_SIZE; // Smallest block size is one page
    while (block_size < size) {
        block_size <<= 1;
        order++;
    }
    return order;
}

// Helper to get the buddy of a given block
static inline uintptr_t get_buddy(uintptr_t addr, int order) {
    uintptr_t block_size = (1 << order) * PAGE_SIZE;
    return addr ^ block_size;
}

// Helper to check if a block is on a free list
static bool is_block_free(uintptr_t addr, int order) {
    for (free_node_t* node = buddy_allocator.free_list[order]; node != NULL; node = node->next) {
        if ((uintptr_t)node == addr) {
            return true;
        }
    }
    return false;
}

// Helper to remove a block from a free list
static void remove_from_list(uintptr_t addr, int order) {
    free_node_t** current = &buddy_allocator.free_list[order];
    while (*current) {
        if ((uintptr_t)*current == addr) {
            *current = (*current)->next;
            return;
        }
        current = &(*current)->next;
    }
}

void buddy_init(void* mem_start, size_t size) {
    buddy_allocator.start_addr = (uintptr_t)mem_start;
    buddy_allocator.total_pages = size / PAGE_SIZE;

    // Allocate space for the page_orders tracking array itself
    // This assumes the first few pages are used for this metadata
    uint32_t metadata_size = buddy_allocator.total_pages * sizeof(uint8_t);
    buddy_allocator.page_orders = (uint8_t*)mem_start;
    memset(buddy_allocator.page_orders, 0xFF, metadata_size); // 0xFF indicates free

    uintptr_t start_of_pages = buddy_allocator.start_addr + metadata_size;
    // Align to the next page boundary
    start_of_pages = (start_of_pages + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

    uintptr_t end_of_mem = buddy_allocator.start_addr + size;
    
    for (int i = 0; i <= MAX_ORDER; i++) {
        buddy_allocator.free_list[i] = NULL;
    }

    // Add all available memory into the free lists, starting with the largest blocks
    uintptr_t current_addr = start_of_pages;
    for (int order = MAX_ORDER; order >= 0; order--) {
        uintptr_t block_size = (1 << order) * PAGE_SIZE;
        while (current_addr + block_size <= end_of_mem) {
            free_node_t* node = (free_node_t*)current_addr;
            node->next = buddy_allocator.free_list[order];
            buddy_allocator.free_list[order] = node;
            current_addr += block_size;
        }
    }
}

void* buddy_alloc(size_t size) {
    int order = get_order(size);
    if (order > MAX_ORDER) {
        return NULL; // Request too large
    }

    // Find a suitable block, splitting larger ones if necessary
    int current_order;
    for (current_order = order; current_order <= MAX_ORDER; current_order++) {
        if (buddy_allocator.free_list[current_order] != NULL) {
            break; // Found a block
        }
    }

    if (current_order > MAX_ORDER) {
        return NULL; // Out of memory
    }

    // Pop the block from its list
    free_node_t* block = buddy_allocator.free_list[current_order];
    buddy_allocator.free_list[current_order] = block->next;

    // Split the block until it's the correct size
    while (current_order > order) {
        current_order--;
        uintptr_t block_size = (1 << current_order) * PAGE_SIZE;
        
        // The other half of the split block becomes the buddy
        free_node_t* buddy = (free_node_t*)((uintptr_t)block + block_size);
        
        // Add the buddy to its free list
        buddy->next = buddy_allocator.free_list[current_order];
        buddy_allocator.free_list[current_order] = buddy;
    }
    
    // Mark the page(s) as used with the correct order
    uint32_t page_index = ((uintptr_t)block - buddy_allocator.start_addr) / PAGE_SIZE;
    buddy_allocator.page_orders[page_index] = order;

    return (void*)block;
}

void buddy_free(void* ptr, size_t size) {
    if (ptr == NULL) return;

    uintptr_t addr = (uintptr_t)ptr;
    int order = get_order(size);

    // Merge with buddies if they are free
    while (order < MAX_ORDER) {
        uintptr_t buddy_addr = get_buddy(addr, order);

        // Check if the buddy is also free
        uint32_t buddy_page_index = (buddy_addr - buddy_allocator.start_addr) / PAGE_SIZE;
        if (buddy_allocator.page_orders[buddy_page_index] != order || !is_block_free(buddy_addr, order)) {
            break; // Buddy is not free or is of a different size, stop merging
        }
        
        // Buddy is free, merge them.
        remove_from_list(buddy_addr, order);
        
        // The new, larger block starts at the lower of the two addresses
        if (buddy_addr < addr) {
            addr = buddy_addr;
        }

        order++; // Move up to the next order
    }

    // Add the final (potentially merged) block to the free list
    free_node_t* node = (free_node_t*)addr;
    node->next = buddy_allocator.free_list[order];
    buddy_allocator.free_list[order] = node;
    
    // Mark the page(s) as free
    uint32_t page_index = (addr - buddy_allocator.start_addr) / PAGE_SIZE;
    buddy_allocator.page_orders[page_index] = 0xFF; // 0xFF indicates free
}
