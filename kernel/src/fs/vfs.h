#ifndef VFS_H
#define VFS_H

#include <stdint.h>
#include <stddef.h>
#include "../security/mac.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_MOUNTPOINT  0x04

struct fs_node;

typedef uint32_t (*read_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef uint32_t (*write_type_t)(struct fs_node*,uint32_t,uint32_t,uint8_t*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent* (*readdir_type_t)(struct fs_node*,uint32_t);
typedef struct fs_node* (*finddir_type_t)(struct fs_node*,char *name);
typedef struct fs_node* (*create_type_t)(struct fs_node*,char *name, uint32_t flags);
typedef struct fs_node* (*mkdir_type_t)(struct fs_node*,char *name, uint32_t flags);


typedef struct fs_node {
    char name[128];
    uint32_t flags;
    uint32_t inode;
    uint32_t length;
    uint32_t uid;
    uint32_t gid;
    uint32_t mask;
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    create_type_t create;
    mkdir_type_t mkdir;
    struct fs_node *ptr;
    security_context_t sec_ctx;
} fs_node_t;

struct dirent {
    char name[128];
    uint32_t ino;
};

extern fs_node_t *fs_root;

void vfs_init();
uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer);
void open_fs(fs_node_t *node, uint8_t read, uint8_t write);
void close_fs(fs_node_t *node);
struct dirent *readdir_fs(fs_node_t *node, uint32_t index);
fs_node_t *finddir_fs(fs_node_t *node, char *name);

#endif
