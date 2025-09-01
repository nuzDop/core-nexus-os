/*
 * LimitlessOS PolyCore Kernel Linker Script
 */

ENTRY(start) /* The entry point is the 'start' label in boot.asm */

SECTIONS
{
    /* Begin putting sections at 1 MiB, a common base for modern kernels. */
    . = 1M;

    .text :
    {
        *(.multiboot)   /* Place the multiboot header first. */
        *(.text)        /* All compiled code (.text sections) goes here. */
    }

    .rodata :
    {
        *(.rodata)      /* All read-only data. */
    }

    .data :
    {
        *(.data)        /* All initialized data. */
    }

    .bss :
    {
        *(.bss)         /* All uninitialized data. */
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}
