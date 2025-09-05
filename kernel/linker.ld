/*
 * LimitlessOS PolyCore Kernel Linker Script for x86_64
 */

ENTRY(start)

SECTIONS
{
    /* Start at 1MB, the physical address where the bootloader will load us. */
    . = 1M;
    
    .text : AT(ADDR(.text))
    {
        *(.multiboot)
        *(.text) /* This will contain the 32-bit start code */
    }
    
    /* This section contains the 64-bit kernel code */
    .text.kmain64 : AT(ADDR(.text.kmain64))
    {
        *(.text.kmain64)
    }

    /* * Higher-half kernel virtual address space starts here. 
     * KASLR will be handled by the bootloader, which randomizes the physical base address.
     * The `.` symbol remains relative to the load address.
     */
    . = 0xFFFFFFFF80000000;
    
    .rodata : ALIGN(4K)
    {
        *(.rodata)
    }

    .data : ALIGN(4K)
    {
        *(.data)
    }

    .bss : ALIGN(4K)
    {
        *(.bss)
    }

    /* Add a section for the stack protector guard value */
    .data.stack_guard : ALIGN(8)
    {
        __stack_chk_guard = .;
        QUAD(0xDEADBEEFCAFEBABE); /* Initial guard value */
    }
}
