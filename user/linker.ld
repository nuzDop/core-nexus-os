/* user/linker.ld */
ENTRY(_start)
SECTIONS
{
    . = 0x400000; /* Standard user-space program base address */
    .text : { *(.text) }
    .data : { *(.data) }
    .bss : { *(.bss) }
}
