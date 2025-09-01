#include "vfs.h"
#include "../mem/pmm.h"

fs_node_t* fs_root = 0; // The root of our filesystem tree

// Initialize the VFS by creating a root node
void init_vfs() {
    // For now, the root is just a placeholder.
    // A filesystem driver (like InfinityFS) will mount itself here.
    fs_root = (fs_node_t*)pmm_alloc_page(); // Using page allocation for simplicity
    if (fs_root) {
        // Copy "root" to the name field
        char* name = "root";
        int i = 0;
        while(name[i] != '\0') {
            fs_root->name[i] = name[i];
            i++;
        }
        fs_root->name[i] = '\0';
        fs_root->flags = FS_DIRECTORY;
    }
}

// Read from a filesystem node
uint32_t read_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }
    return 0;
}

// Write to a filesystem node
uint32_t write_fs(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }
    return 0;
}
