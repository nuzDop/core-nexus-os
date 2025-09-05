#pragma once

#include <stdint.h>
#include <stddef.h>
#include "../boot/limine.h"

void pmm_init(struct limine_memmap_response *memmap);
void *pmm_alloc(size_t size);
void pmm_free(void *ptr, size_t size);
