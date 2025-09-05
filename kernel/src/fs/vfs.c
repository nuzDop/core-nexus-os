#include "vfs.h"
#include "../mem/pmm.h"
#include "../lib/string.h"

fs_node_t *fs_root = 0;

void vfs_init() {
    fs_root = (fs_node_t*)pmm_alloc_page();
    strcpy(fs_root->name, "/");
    fs_root->flags = FS_DIRECTORY | FS_MOUNTPOINT;
    fs_root->ptr = fs_root;
}

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node->read) {
        return node->read(node, offset, size, buffer);
    }
    return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer) {
    if (node->write) {
        return node->write(node, offset, size, buffer);
    }
    return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write) {
    if (node->open) {
        node->open(node);
    }
}

void close_fs(fs_node_t *node) {
    if (node->close) {
        node->close(node);
    }
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index) {
    if ((node->flags & FS_DIRECTORY) && node->readdir) {
        return node->readdir(node, index);
    }
    return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name) {
    if ((node->flags & FS_DIRECTORY) && node->finddir) {
        return node->finddir(node, name);
    }
    return 0;
}
