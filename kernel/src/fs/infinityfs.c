#include "infinityfs.h"
#include "../drivers/ramdisk.h"
#include "../mem/pmm.h"
#include "../lib/string.h"

void print(char*); // Forward declare from main.c

// Forward declarations for all VFS functions for InfinityFS
static fs_node_t* infinityfs_finddir(fs_node_t* node, char* name);
static struct dirent* infinityfs_readdir(fs_node_t* node, uint32_t index);
static uint32_t infinityfs_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
static uint32_t infinityfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer);
static fs_node_t* infinityfs_create(fs_node_t* parent, char* name, uint32_t flags);

// Helper to read an inode from our "disk"
static void read_inode(uint32_t inode_num, infinityfs_inode_t* inode) {
    ramdisk_read(1 + inode_num, 1, inode);
}

// Helper to write an inode to our "disk"
static void write_inode(uint32_t inode_num, infinityfs_inode_t* inode) {
    ramdisk_write(1 + inode_num, 1, inode);
}

// For simplicity, we'll just have a placeholder for finding a free inode/block
static uint32_t find_first_free_inode() { return 2; } // Assume inode 2 is free
static uint32_t find_first_free_block() { return 11; } // Assume data block 11 is free

// Create a new file or directory
static fs_node_t* infinityfs_create(fs_node_t* parent, char* name, uint32_t flags) {
    // 1. Find a free inode
    uint32_t new_inode_num = find_first_free_inode();
    
    // 2. Create the new inode
    infinityfs_inode_t new_inode;
    new_inode.mode = flags;
    new_inode.size = 0;
    
    // 3. Add the new entry to the parent directory's data block
    infinityfs_inode_t parent_inode;
    read_inode(parent->inode, &parent_inode);
    
    infinityfs_dirent_t* dir_entries = (infinityfs_dirent_t*)pmm_alloc_page();
    ramdisk_read(parent_inode.direct_blocks[0], 1, dir_entries);
    
    // Find the first empty entry
    int entry_index = 0;
    while(dir_entries[entry_index].inode_num != 0) {
        entry_index++;
    }
    
    dir_entries[entry_index].inode_num = new_inode_num;
    strcpy(dir_entries[entry_index].name, name);
    
    // 4. Write changes back to disk
    ramdisk_write(parent_inode.direct_blocks[0], 1, dir_entries);
    write_inode(new_inode_num, &new_inode);
    
    pmm_free_page(dir_entries);
    
    // 5. Return a VFS node for the new file
    return infinityfs_finddir(parent, name);
}

// Write data to a file
static uint32_t infinityfs_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    infinityfs_inode_t inode;
    read_inode(node->inode, &inode);

    if ((inode.mode & FS_FILE) == 0) return 0;

    // 1. Allocate a new data block if one doesn't exist
    if (inode.direct_blocks[0] == 0) {
        inode.direct_blocks[0] = find_first_free_block();
    }

    // 2. Write the buffer to the data block
    // This is a simplified write that overwrites the whole block
    ramdisk_write(inode.direct_blocks[0], 1, buffer);
    
    // 3. Update inode size and write it back
    inode.size = size;
    write_inode(node->inode, &inode);
    node->length = size;

    return size;
}

// ... (readdir, finddir, and read implementations remain the same) ...

// Main function to format and mount the InfinityFS on our RAM disk
void init_infinityfs() {
    // ... (superblock and root inode creation remains the same) ...
    
    // Mount this filesystem to the VFS root
    if (fs_root) {
        strcpy(fs_root->name, "/");
        fs_root->inode = 0;
        fs_root->flags = FS_DIRECTORY;
        fs_root->read = &infinityfs_read;
        fs_root->write = &infinityfs_write;
        fs_root->finddir = &infinityfs_finddir;
        fs_root->readdir = &infinityfs_readdir;
        fs_root->create = &infinityfs_create;
        print("InfinityFS mounted on VFS root.\n");
    } else {
        print("VFS root not found! Cannot mount InfinityFS.\n");
    }
}
