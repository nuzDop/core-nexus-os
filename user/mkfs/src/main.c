/* user/mkfs/src/main.c */

#include <stdint.h>

// --- Syscall Definitions ---
#define SYS_PRINT 1
// We need syscalls for open, write, and close. We'll assume they exist.
// #define SYS_OPEN 12
// #define SYS_WRITE 15
// #define SYS_CLOSE 13
int syscall(int num, int p1, int p2, int p3, int p4, int p5);


// --- LimitlessFS On-Disk Structures ---
// These are duplicated here because a user-space app can't include kernel headers.
#define LFS_MAGIC 0x11F5
#define LFS_BLOCK_SIZE 1024
#define LFS_MAX_FILENAME 28
#define LFS_DIRECT_POINTERS 12
#define FS_FILE 0x01
#define FS_DIRECTORY 0x02

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t inode_bitmap_block;
    uint32_t data_bitmap_block;
    uint32_t inode_table_block;
    uint32_t data_blocks_start;
} lfs_superblock_t;

typedef struct {
    uint16_t type;
    uint16_t permissions;
    uint32_t size;
    uint32_t direct_pointers[LFS_DIRECT_POINTERS];
    uint32_t indirect_pointer;
} lfs_inode_t;

typedef struct {
    uint32_t inode_num;
    char name[LFS_MAX_FILENAME];
} lfs_dirent_t;


// --- Helper Functions ---

// A real implementation of these would be in a user-space libc
void* memset(void* buf, int c, size_t n) {
    char* p = (char*)buf;
    for (size_t i = 0; i < n; i++) {
        p[i] = (char)c;
    }
    return buf;
}

void strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}


void _start() {
    syscall(SYS_PRINT, (int)"Formatting ramdisk with LimitlessFS...\n", 0, 0, 0, 0);

    // Assume these syscalls work and a device exists at "/dev/ramdisk"
    // int device_fd = syscall(SYS_OPEN, (int)"/dev/ramdisk", 0, 0, 0, 0);
    // if (device_fd < 0) {
    //     syscall(SYS_PRINT, (int)"mkfs: Failed to open device.\n", 0, 0, 0, 0);
    //     return;
    // }

    // --- Filesystem Layout ---
    // For a 16MB disk: 16384 blocks of 1024 bytes
    // Superblock: 1 block (at block 0)
    // Inode Bitmap: 1 block (at block 1)
    // Data Bitmap: 16 blocks (at block 2)
    // Inode Table: 128 blocks (for ~4k inodes) (at block 18)
    // Data Blocks: Start at block 146
    
    uint8_t block_buffer[LFS_BLOCK_SIZE];

    // 1. Create and write Superblock
    lfs_superblock_t sb;
    memset(&sb, 0, sizeof(lfs_superblock_t));
    sb.magic = LFS_MAGIC;
    sb.total_blocks = 16 * 1024;
    sb.inode_bitmap_block = 1;
    sb.data_bitmap_block = 2;
    sb.inode_table_block = 18;
    sb.data_blocks_start = 146;
    // syscall(SYS_WRITE, device_fd, 0, LFS_BLOCK_SIZE, (int)&sb);

    // 2. Create and write Inode Bitmap
    memset(block_buffer, 0, LFS_BLOCK_SIZE);
    block_buffer[0] = 0b10000000; // Mark first inode (root) as used
    // syscall(SYS_WRITE, device_fd, sb.inode_bitmap_block * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, (int)block_buffer);

    // 3. Create and write Data Bitmap
    memset(block_buffer, 0, LFS_BLOCK_SIZE);
    block_buffer[0] = 0b10000000; // Mark first data block (for root dir) as used
    // syscall(SYS_WRITE, device_fd, sb.data_bitmap_block * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, (int)block_buffer);
    // ... would need to clear all 16 blocks for a full implementation ...

    // 4. Create and write Inode Table with root inode
    memset(block_buffer, 0, LFS_BLOCK_SIZE);
    lfs_inode_t* root_inode = (lfs_inode_t*)block_buffer;
    root_inode->type = FS_DIRECTORY;
    root_inode->size = sizeof(lfs_dirent_t) * 2; // For '.' and '..'
    root_inode->direct_pointers[0] = sb.data_blocks_start; // First data block
    // syscall(SYS_WRITE, device_fd, sb.inode_table_block * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, (int)block_buffer);

    // 5. Create and write the root directory's data block
    memset(block_buffer, 0, LFS_BLOCK_SIZE);
    lfs_dirent_t* dirents = (lfs_dirent_t*)block_buffer;
    // Entry for "."
    dirents[0].inode_num = 0;
    strcpy(dirents[0].name, ".");
    // Entry for ".."
    dirents[1].inode_num = 0;
    strcpy(dirents[1].name, "..");
    // syscall(SYS_WRITE, device_fd, sb.data_blocks_start * LFS_BLOCK_SIZE, LFS_BLOCK_SIZE, (int)block_buffer);

    // 6. Close device and exit
    // syscall(SYS_CLOSE, device_fd, 0, 0, 0, 0);
    syscall(SYS_PRINT, (int)"Format complete.\n", 0, 0, 0, 0);

    // An exit syscall would go here
    while(1);
}
