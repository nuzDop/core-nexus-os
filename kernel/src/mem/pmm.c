#include "pmm.h"
#include "buddy.h"

void pmm_init(struct limine_memmap_response *memmap) {
    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
        if (entry->type == LIMINE_MEMMAP_USABLE) {
            buddy_init((void *)entry->base, entry->length);
        }
    }
}

void *pmm_alloc(size_t size) {
    return buddy_alloc(size);
}

void pmm_free(void *ptr, size_t size) {
    buddy_free(ptr, size);
}
