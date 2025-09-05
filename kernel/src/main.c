#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#include "gui/compositor.h"
#include "proc/elf.h"

extern uint8_t _binary_user_shell_elf_start[];
extern uint8_t _binary_user_shell_elf_end[];

extern uint8_t _binary_user_guitest_elf_start[];
extern uint8_t _binary_user_guitest_elf_end[];

void kmain(multiboot_info_t* mbd, uint32_t magic) {
    init_gdt();
    init_idt();
    pmm_init(mbd);
    vmm_init();
    crypto_init();
    init_syscalls();
    init_vfs();
    ramdisk_init(4 * 1024 * 1024);
    init_infinityfs();
    init_tasking();
    init_timer(100);
    acpi_init();
    vbe_init(1024, 768, 32);
    compositor_init(); // Use the new compositor
    init_mouse();
    init_keyboard();
    // ... other initializations

    // Load the new shell application instead of the browser
    fs_node_t* shell_file = fs_root->create(fs_root, "shell.elf", FS_FILE);
    uint32_t shell_size = _binary_user_shell_elf_end - _binary_user_shell_elf_start;
    shell_file->write(shell_file, 0, shell_size, _binary_user_shell_elf_start);

    // Load guitest app
    fs_node_t* guitest_file = fs_root->create(fs_root, "guitest.elf", FS_FILE);
    uint32_t guitest_size = _binary_user_guitest_elf_end - _binary_user_guitest_elf_start;
    guitest_file->write(guitest_file, 0, guitest_size, _binary_user_guitest_elf_start);

    page_directory_t* shell_dir = clone_directory(kernel_directory);
    uint32_t entry_point = elf_load("shell.elf", shell_dir);

    if (!entry_point) { while(1); }
    
    __asm__ __volatile__ ("sti");
    switch_page_directory(shell_dir);
    enter_user_mode(entry_point);
    
    while(1) { compositor_redraw(); }
}
