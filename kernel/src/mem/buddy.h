#pragma once

#include <stdint.h>
#include <stddef.h>

#define MAX_ORDER 10

void buddy_init(void *mem, size_t size);
void *buddy_alloc(size_t size);
void buddy_free(void *ptr, size_t size);
