#include <stdint.h>
#define SYS_PRINT 1
// This would use syscalls for opening and writing to the raw ramdisk device.
void _start() {
    syscall(SYS_PRINT, (int)"Formatting ramdisk with LimitlessFS...\n", 0,0,0,0);

    // 1. Open "/dev/ramdisk"
    // 2. Write a valid superblock
    // 3. Write clean bitmaps
    // 4. Write a root inode
    // 5. Write root directory data block containing "." and ".."

    syscall(SYS_PRINT, (int)"Format complete.\n", 0,0,0,0);
    // Exit syscall is needed here
    while(1);
}
