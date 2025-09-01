#ifndef INFINITYFS_H
#define INFINITYFS_H

#include <stdint.h>
#include "vfs.h"

#define INFINITYFS_MAGIC 0x1F51F51F // "IFS" in a fun way

// The Superblock: Contains metadata about the entire filesystem.
typedef struct {
    uint32_t magic;
    uint32_t version;
    uint64_t total_blocks;
    uint64_t used_blocks;
    uint32_t inode_count;
    uint32_t block_size;
    
    uint64_t inode_table_start;
    uint64_t data_block_start;
    uint64_t root_inode;

    uint64_t free_block_bitmap; // LBA of the free block bitmap
    uint64_t free_inode_bitmap; // LBA of the free inode bitmap

} infinityfs_superblock_t;

// An Inode: Represents a single file or directory.
typedef struct {
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint64_t size;
    uint64_t atime;
    uint64_t mtime;
    uint64_t ctime;
    uint64_t direct_blocks[12];
    uint64_t indirect_block;
    uint64_t doubly_indirect_block;
    uint32_t ref_count;
    uint32_t snapshot_id;
} infinityfs_inode_t;

// A directory entry structure.
typedef struct {
    uint32_t inode_num;
    char name[124];
} infinityfs_dirent_t;

// Public function to initialize and mount an InfinityFS partition
void init_infinityfs();

// Public VFS functions that will be implemented in infinityfs.c
fs_node_t* infinityfs_create(fs_node_t* parent, char* name, uint32_t flags);
uint32_t infinityfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);

#endif
