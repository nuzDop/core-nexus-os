#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "mem/pmm.h"
#include "mem/vmm.h"
#include "proc/task.h"
#include "interrupts/timer.h"
#include "interrupts/syscall.h"
#include "fs/vfs.h"
#include "drivers/ramdisk.h"
#include "fs/infinityfs.h"
#include "drivers/vbe.h"
#include "drivers/mouse.h"
#include "drivers/keyboard.h"
#include "drivers/npu.h"
#include "drivers/rtl8139.h"
#include "gui/window.h"
#include "proc/elf.h"
#include "ai/nexus_core.h"
#include "net/net.h"
#include "net/sockets.h"

void keyboard_handler_c();
void interrupt_handler_c(registers_t regs);
window_t* create_window(int x, int y, int w, int h, char* title);
extern uint8_t _binary_user_browser_elf_start[];
extern uint8_t _binary_user_browser_elf_end[];

void kmain(void) {
    init_gdt();
    init_idt();
    pmm_init(128 * 1024 * 1024);
    vmm_init();
    init_syscalls();
    init_vfs();
    ramdisk_init(4 * 1024 * 1024);
    init_infinityfs();
    init_tasking();
    init_timer(100);
    vbe_init(1024, 768, 32);
    init_window_manager();
    init_mouse();
    init_keyboard();
    npu_init();
    nexus_core_init();
    net_init();
    sockets_init();
    rtl8139_init();
    
    // Create a file for the browser executable
    fs_node_t* browser_file = fs_root->create(fs_root, "browser.elf", FS_FILE);
    uint32_t browser_size = _binary_user_browser_elf_end - _binary_user_browser_elf_start;
    browser_file->write(browser_file, 0, browser_size, _binary_user_browser_elf_start);

    // Create a memory space for the browser process
    page_directory_t* browser_dir = clone_directory(kernel_directory);
    uint32_t entry_point = elf_load("browser.elf", browser_dir);

    if (!entry_point) { while(1); }
    
    // Start the browser
    __asm__ __volatile__ ("sti");
    switch_page_directory(browser_dir);
    enter_user_mode(entry_point);
    
    while(1) { compositor_redraw(); }
}
