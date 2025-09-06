#include <stddef.h>
#include <stdint.h>

// A rudimentary heap implementation for the C library.
// This is a simple first-fit allocator, and is not thread-safe.

#define HEAP_SIZE 0x100000 // 1MB heap

static uint8_t heap[HEAP_SIZE];
static uint8_t* heap_ptr = heap;

void* malloc(size_t size) {
    if (heap_ptr + size > heap + HEAP_SIZE) {
        return NULL; // Out of memory
    }
    void* ptr = heap_ptr;
    heap_ptr += size;
    return ptr;
}

void free(void* ptr) {
    // This simple allocator does not support freeing memory.
    (void)ptr;
}

void* calloc(size_t num, size_t size) {
    size_t total_size = num * size;
    void* ptr = malloc(total_size);
    if (ptr) {
        for (size_t i = 0; i < total_size; ++i) {
            ((uint8_t*)ptr)[i] = 0;
        }
    }
    return ptr;
}

void* realloc(void* ptr, size_t size) {
    if (!ptr) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    // This simple allocator does not support reallocating memory.
    return NULL;
}
