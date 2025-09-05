#include <stdint.h>
#define SYS_CREATE_WINDOW 4
#define SYS_PRINT 1
// ... other syscalls

void _start() {
    syscall(SYS_CREATE_WINDOW, 100, 100, 500, 350, (int)"LimitlessOS Installer");

    syscall(SYS_PRINT, (int)"Welcome to the Installer!\n");
    // 1. Create GUI with widgets for "Next", "Cancel".
    // 2. On "Next", show a placeholder for disk partitioning.
    // 3. On "Next", format the selected partition with LimitlessFS using mkfs logic.
    // 4. Copy kernel and user programs from the live ramdisk to the new partition.
    // 5. Write a bootloader configuration to the disk.
    // 6. Show "Installation Complete" message.
    
    while(1);
}
