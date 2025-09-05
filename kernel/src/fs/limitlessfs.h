#ifndef LIMITLESSFS_H
#define LIMITLESSFS_H

#include <stdint.h>
#include "vfs.h"

#define LFS_MAGIC 0x11F5 // LimitlessFS Magic Number
#define LFS_BLOCK_SIZE 1024
#define LFS_MAX_FILENAME 28
#define LFS_DIRECT_POINTERS 12

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_block;
    uint32_t data_blocks_start;
} lfs_superblock_t;

typedef struct {
    uint16_t type; // FS_FILE or FS_DIRECTORY
    uint16_t permissions;
    uint32_t size;
    uint32_t direct_pointers[LFS_DIRECT_POINTERS];
    uint32_t indirect_pointer;
} lfs_inode_t;

typedef struct {
    uint32_t inode_num;
    char name[LFS_MAX_FILENAME];
} lfs_dirent_t;

fs_node_t* mount_limitlessfs(fs_node_t* device);

#endif
