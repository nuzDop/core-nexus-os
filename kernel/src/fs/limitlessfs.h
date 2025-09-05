#ifndef LIMITLESSFS_H
#define LIMITLESSFS_H

#include <stdint.h>
#include "../sync/spinlock.h"
#include "vfs.h"

#define LFS_MAGIC 0x11F5 // LimitlessFS Magic Number
#define LFS_BLOCK_SIZE 4096
#define LFS_MAX_FILENAME 252
#define LFS_DIRECT_POINTERS 12
#define LFS_INDIRECT_POINTERS (LFS_BLOCK_SIZE / sizeof(uint32_t))

// --- On-Disk Journal Structures ---
#define LFS_JOURNAL_MAGIC 0xJ001
typedef enum {
    JOURNAL_BLOCK_INODE,
    JOURNAL_BLOCK_DATA_BITMAP,
    JOURNAL_BLOCK_INODE_BITMAP
} journal_block_type_t;

typedef struct {
    uint32_t block_num;      // Original on-disk block number
    journal_block_type_t type;
} journal_block_header_t;

typedef struct {
    uint32_t magic;
    uint32_t sequence_id;
    uint32_t num_blocks; // Number of metadata blocks in this transaction
} journal_transaction_header_t;


// --- On-Disk Filesystem Structures ---
typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t journal_start_block;
    uint32_t journal_num_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_block;
    uint32_t data_blocks_start;
} lfs_superblock_t;

typedef struct {
    uint16_t type; // FS_FILE or FS_DIRECTORY
    uint16_t permissions;
    uint32_t uid;
    uint32_t gid;
    uint64_t size;
    uint64_t last_modified;
    uint64_t security_context_id;
    uint32_t direct_pointers[LFS_DIRECT_POINTERS];
    uint32_t indirect_pointer;
    uint32_t double_indirect_pointer;
} lfs_inode_t;

typedef struct {
    uint32_t inode_num;
    char name[LFS_MAX_FILENAME];
} lfs_dirent_t;

// --- Public Driver Functions ---
void limitlessfs_init();
fs_node_t* limitlessfs_mount(fs_node_t* device);

#endif
