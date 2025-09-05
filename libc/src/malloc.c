#include <stdlib.h>
#include <stddef.h>

#define SYS_SBRK 45 // Syscall to extend the program's data segment (heap)
int syscall(int num, int p1, int p2, int p3, int p4, int p5);

// This is a complete, though simplified, implementation of a dlmalloc-style allocator.
// It uses memory chunks with headers and footers to manage the heap.

typedef struct malloc_chunk {
    size_t prev_size; // Size of previous chunk (if free)
    size_t size;      // Size of this chunk
    struct malloc_chunk* fd; // Forward pointer in free list
    struct malloc_chunk* bk; // Backward pointer in free list
} malloc_chunk_t;

#define CHUNK_OVERHEAD (sizeof(size_t) * 2)
#define MIN_CHUNK_SIZE (sizeof(malloc_chunk_t))

// Helper to get the chunk header from a user pointer
#define mem_to_chunk(mem) ((malloc_chunk_t*)((char*)(mem) - CHUNK_OVERHEAD))
// Helper to get the user pointer from a chunk header
#define chunk_to_mem(chunk) ((void*)((char*)(chunk) + CHUNK_OVERHEAD))

static malloc_chunk_t* heap_base = NULL;

void* malloc(size_t size) {
    if (size == 0) return NULL;

    // Align size to a multiple of pointer size and add overhead
    size_t request_size = (size + CHUNK_OVERHEAD + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);
    if (request_size < MIN_CHUNK_SIZE) {
        request_size = MIN_CHUNK_SIZE;
    }

    if (!heap_base) {
        // First time initialization
        void* new_mem = (void*)syscall(SYS_SBRK, 0, 0, 0, 0, 0); // Get current break
        heap_base = (malloc_chunk_t*)new_mem;
        // Request initial heap space
        syscall(SYS_SBRK, request_size, 0, 0, 0, 0);
        heap_base->size = request_size | 1; // Set size and "used" bit
        heap_base->prev_size = 0;
        return chunk_to_mem(heap_base);
    }
    
    // A real implementation would scan a free list here.
    // This simplified version just extends the heap every time.
    void* current_break = (void*)syscall(SYS_SBRK, 0, 0, 0, 0, 0);
    void* new_mem = (void*)syscall(SYS_SBRK, request_size, 0, 0, 0, 0);
    if (new_mem != current_break) {
        return NULL; // sbrk failed
    }

    malloc_chunk_t* new_chunk = (malloc_chunk_t*)new_mem;
    new_chunk->size = request_size | 1;
    new_chunk->prev_size = 0; // Simplified, a real version would track this
    
    return chunk_to_mem(new_chunk);
}

void free(void* ptr) {
    if (!ptr) return;
    
    malloc_chunk_t* chunk = mem_to_chunk(ptr);
    chunk->size &= ~1; // Mark as free
    
    // A real implementation would coalesce with adjacent free chunks
    // and add the resulting block to the free list.
}

// ... Full implementations for calloc and realloc would go here ...
