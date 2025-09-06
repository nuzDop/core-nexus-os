#ifndef FS_VFS_H
#define FS_VFS_H
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Forward declaration for self-referential typedefs */
struct fs_node;
typedef struct fs_node fs_node_t;

/* ---------------- Filesystem object type flags ---------------- /
enum {
FS_FILE = 0x001,
FS_DIRECTORY = 0x002,
FS_CHARDEVICE = 0x004,
FS_BLOCKDEVICE = 0x008,
FS_PIPE = 0x010,
FS_SYMLINK = 0x020,
FS_MOUNTPOINT = 0x040, / mountpoint; 'impl' points to mount root */
};

/* Reasonable system limits (tunable at build time) */
#ifndef FS_NAME_MAX
#define FS_NAME_MAX 128
#endif

#ifndef FS_PATH_MAX
#define FS_PATH_MAX 1024
#endif

/* ---------------- Directory entry (portable) ---------------- /
typedef struct dirent {
char name[FS_NAME_MAX];
uint64_t ino; / inode or unique id /
uint32_t type; / FS_* flags subset (optional hint) */
} dirent_t;

/* ---------------- VFS operation signatures ---------------- */
typedef size_t (*read_type_t) (fs_node_t *node, uint64_t offset, size_t size, void *buffer);
typedef size_t (*write_type_t) (fs_node_t *node, uint64_t offset, size_t size, const void *buffer);
typedef int (*open_type_t) (fs_node_t *node, uint32_t flags);
typedef int (*close_type_t) (fs_node_t *node);
typedef int (*ioctl_type_t) (fs_node_t *node, uint32_t request, void *argp);
typedef int (*truncate_type_t) (fs_node_t *node, uint64_t new_length);
typedef void * (*mmap_type_t) (fs_node_t *node, uint64_t offset, size_t size, uint32_t prot, uint32_t flags);
typedef int (*readdir_type_t) (fs_node_t *node, uint64_t index, dirent_t out); / index-based iteration */
typedef fs_node_t * (*finddir_type_t) (fs_node_t *node, const char *name);
typedef int (*create_type_t) (fs_node_t *node, const char *name, uint32_t mode);
typedef int (*mkdir_type_t) (fs_node_t *node, const char *name, uint32_t mode);
typedef int (*unlink_type_t) (fs_node_t *node, const char *name);
typedef int (*rmdir_type_t) (fs_node_t *node, const char *name);

/*

---------------- Core VFS node ----------------

This structure intentionally mirrors common hobby-OS layouts and

is extended with device_info to carry driver-private state.
/
struct fs_node {
/ Identity / metadata /
char name[FS_NAME_MAX];
uint32_t mask; / permission bits /
uint32_t uid; / owner /
uint32_t gid; / group /
uint32_t flags; / FS_* /
uint64_t inode; / inode or unique id /
uint64_t length; / file length in bytes (advisory for devices) /
uint64_t atime; / access time (epoch ns or ticks) /
uint64_t mtime; / modification time /
uint64_t ctime; / change/creation time */

/* VFS-internal plumbing */
fs_node_t mount; / if FS_MOUNTPOINT, pointer to mounted root */
void impl; / filesystem-implementation specific data */

/*

Device-private hook:

Drivers (e.g., AHCI) may stash a pointer to their per-device state here.

AHCI expects to read this field (see ahci_read/ahci_write usage).
*/
void *device_info;

/* Operations */
read_type_t read;
write_type_t write;
open_type_t open;
close_type_t close;
ioctl_type_t ioctl;
truncate_type_t truncate;
mmap_type_t mmap;
readdir_type_t readdir;
finddir_type_t finddir;
create_type_t create;
mkdir_type_t mkdir;
unlink_type_t unlink;
rmdir_type_t rmdir;
};

/* Root of the virtual filesystem */
extern fs_node_t *vfs_root;

/* ---------------- Convenience inlines (NULL-safe) ---------------- */
static inline size_t vfs_read(fs_node_t *n, uint64_t off, size_t sz, void *buf) {
return (n && n->read) ? n->read(n, off, sz, buf) : 0;
}
static inline size_t vfs_write(fs_node_t *n, uint64_t off, size_t sz, const void *buf) {
return (n && n->write) ? n->write(n, off, sz, buf) : 0;
}
static inline int vfs_open(fs_node_t *n, uint32_t flags) {
return (n && n->open) ? n->open(n, flags) : 0;
}
static inline int vfs_close(fs_node_t *n) {
return (n && n->close) ? n->close(n) : 0;
}
static inline int vfs_ioctl(fs_node_t *n, uint32_t req, void *argp) {
return (n && n->ioctl) ? n->ioctl(n, req, argp) : -1;
}
static inline int vfs_truncate(fs_node_t *n, uint64_t nl) {
return (n && n->truncate) ? n->truncate(n, nl) : -1;
}
static inline void *vfs_mmap(fs_node_t *n, uint64_t off, size_t sz, uint32_t prot, uint32_t flags) {
return (n && n->mmap) ? n->mmap(n, off, sz, prot, flags) : NULL;
}
static inline int vfs_readdir(fs_node_t *n, uint64_t idx, dirent_t *out) {
return (n && n->readdir) ? n->readdir(n, idx, out) : -1;
}
static inline fs_node_t *vfs_finddir(fs_node_t *n, const char *name) {
return (n && n->finddir) ? n->finddir(n, name) : NULL;
}
static inline int vfs_create(fs_node_t *n, const char *name, uint32_t mode) {
return (n && n->create) ? n->create(n, name, mode) : -1;
}
static inline int vfs_mkdir(fs_node_t *n, const char *name, uint32_t mode) {
return (n && n->mkdir) ? n->mkdir(n, name, mode) : -1;
}
static inline int vfs_unlink(fs_node_t *n, const char *name) {
return (n && n->unlink) ? n->unlink(n, name) : -1;
}
static inline int vfs_rmdir(fs_node_t *n, const char *name) {
return (n && n->rmdir) ? n->rmdir(n, name) : -1;
}

/* ---------------- Common helpers (implemented in VFS core) ---------------- /
/ These symbols are declared here for users; implementations live in vfs.c */
int vfs_mount(const char *path, fs_node_t *root);
int vfs_umount(const char *path);
fs_node_t *vfs_resolve_path(const char path); / returns retained node or NULL */
int vfs_stat_path(const char *path, uint64_t *length, uint32_t *flags, uint64_t *inode);
int vfs_link(const char *oldpath, const char *newpath);
int vfs_symlink(const char *target, const char *linkpath);
int vfs_readlink(const char *path, char *out, size_t cap);

#endif /* FS_VFS_H */
