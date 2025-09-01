#ifndef RAMDISK_H
#define RAMDISK_H

#include <stdint.h>
#include <stddef.h>

// Initializes the RAM disk with a given size in bytes.
void ramdisk_init(size_t size);

// Reads a block of data from the RAM disk.
// block_no: The block number to read from.
// count: The number of blocks to read.
// buffer: The destination buffer to write data into.
void ramdisk_read(uint32_t block_no, uint32_t count, void* buffer);

// Writes a block of data to the RAM disk.
void ramdisk_write(uint32_t block_no, uint32_t count, void* buffer);

#endif
