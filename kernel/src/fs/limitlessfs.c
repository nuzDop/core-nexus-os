/* kernel/src/fs/limitlessfs.c */

#include "limitlessfs.h"
#include "../mem/pmm.h"
#include "../lib/string.h"
#include "../drivers/ramdisk.h" // For now, we assume this is our block device

// A structure to hold in-memory information about a mounted LFS partition
typedef struct {
    lfs_superblock_t sb;
    fs_node_t* device;
    uint8_t* inode_bitmap;
    uint8_t* data_bitmap;
} lfs_mount_info_t;

// --- Internal Helper Functions ---

// Reads a block from the underlying device
static void lfs_read_block(lfs_mount_info_t* info, uint32_t block_num, uint8_t* buf) {
    read_fs(info->device, block_num * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, buf);
}

// Writes a block to the underlying device
static void lfs_write_block(lfs_mount_info_t* info, uint32_t block_num, uint8_t* buf) {
    write_fs(info->device, block_num * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, buf);
}

// Reads an inode from the inode table
static void lfs_read_inode(lfs_mount_info_t* info, uint32_t inode_num, lfs_inode_t* inode_buf) {
    uint32_t block_num = info->sb.inode_table_block + (inode_num * sizeof(lfs_inode_t)) / LFS_BLOCK_SIZE;
    uint32_t offset = (inode_num * sizeof(lfs_inode_t)) % LFS_BLOCK_SIZE;
    
    uint8_t* buffer = (uint8_t*)pmm_alloc_page();
    lfs_read_block(info, block_num, buffer);
    memcpy(inode_buf, buffer + offset, sizeof(lfs_inode_t));
    pmm_free_page(buffer);
}


// --- VFS Interface Functions ---

static struct dirent dirent; // Static dirent to return from readdir

struct dirent* limitlessfs_readdir(fs_node_t* node, uint32_t index) {
    lfs_mount_info_t* info = (lfs_mount_info_t*)node->device_info;
    lfs_inode_t inode;
    lfs_read_inode(info, node->inode, &inode);

    if (!(inode.type & FS_DIRECTORY)) return NULL;

    uint8_t* buffer = (uint8_t*)pmm_alloc_page();
    lfs_read_block(info, inode.direct_pointers[0], buffer); // Assume small directories for now
    
    lfs_dirent_t* dirents = (lfs_dirent_t*)buffer;
    uint32_t dirent_count = inode.size / sizeof(lfs_dirent_t);

    if (index >= dirent_count) {
        pmm_free_page(buffer);
        return NULL;
    }

    strcpy(dirent.name, dirents[index].name);
    dirent.ino = dirents[index].inode_num;
    
    pmm_free_page(buffer);
    return &dirent;
}

fs_node_t* limitlessfs_finddir(fs_node_t* node, char* name) {
    lfs_mount_info_t* info = (lfs_mount_info_t*)node->device_info;
    lfs_inode_t dir_inode;
    lfs_read_inode(info, node->inode, &dir_inode);

    if (!(dir_inode.type & FS_DIRECTORY)) return NULL;

    uint8_t* buffer = (uint8_t*)pmm_alloc_page();
    lfs_read_block(info, dir_inode.direct_pointers[0], buffer);

    lfs_dirent_t* dirents = (lfs_dirent_t*)buffer;
    uint32_t dirent_count = dir_inode.size / sizeof(lfs_dirent_t);

    for (uint32_t i = 0; i < dirent_count; i++) {
        if (strcmp(dirents[i].name, name) == 0) {
            lfs_inode_t file_inode;
            lfs_read_inode(info, dirents[i].inode_num, &file_inode);

            fs_node_t* new_node = (fs_node_t*)pmm_alloc_page();
            memset(new_node, 0, sizeof(fs_node_t));
            strcpy(new_node->name, dirents[i].name);
            new_node->inode = dirents[i].inode_num;
            new_node->length = file_inode.size;
            new_node->flags = file_inode.type;
            new_node->device_info = info; // Crucial for subsequent calls
            
            // Hook up function pointers
            new_node->readdir = &limitlessfs_readdir;
            new_node->finddir = &limitlessfs_finddir;
            // ... read, write, etc. ...

            pmm_free_page(buffer);
            return new_node;
        }
    }

    pmm_free_page(buffer);
    return NULL;
}


fs_node_t* mount_limitlessfs(fs_node_t* device) {
    lfs_mount_info_t* info = (lfs_mount_info_t*)pmm_alloc_page();
    info->device = device;
    
    // Read superblock
    lfs_read_block(info, 0, (uint8_t*)&info->sb);
    
    if (info->sb.magic != LFS_MAGIC) {
        pmm_free_page(info);
        return NULL; // Invalid filesystem
    }
    
    // Create root VFS node
    fs_node_t* root_node = (fs_node_t*)pmm_alloc_page();
    memset(root_node, 0, sizeof(fs_node_t));
    strcpy(root_node->name, "/");
    root_node->inode = 0; // Root directory is always inode 0
    root_node->flags = FS_DIRECTORY;
    root_node->device_info = info; // Link to our mount info

    // Hook up the VFS functions
    root_node->readdir = &limitlessfs_readdir;
    root_node->finddir = &limitlessfs_finddir;
    // ... read, write, create, mkdir ...

    return root_node;
}
