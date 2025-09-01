# LimitlessOS x86_64 Bootloader
# This file performs the transition to 64-bit long mode.

.set MAGIC,    0x1BADB002
.set FLAGS,    0x03 # Request memory map and video info from GRUB
.set CHECKSUM, -(MAGIC + FLAGS)

.section .multiboot
.align 4
    .long MAGIC
    .long FLAGS
    .long CHECKSUM

.section .bootstrap_stack, "aw", @nobits
.align 16
stack_bottom:
.skip 16384 # 16 KB stack
stack_top:

.section .text
.global start
.extern kmain32 # The C entry point for 32-bit setup
.extern gdt64_pointer # The GDT pointer for our 64-bit GDT

.code32
start:
    # Set up a temporary stack
    mov $stack_top, %esp

    # Push multiboot info for the C kernel
    push %ebx 
    push %eax

    # Call our 32-bit C entry point to set up page tables
    call kmain32
    
    # kmain32 will return the physical address of the PML4 in EAX.
    # 1. Load the PML4 into CR3
    mov %eax, %cr3

    # 2. Enable PAE (Physical Address Extension)
    mov %cr4, %eax
    or $0x20, %eax
    mov %eax, %cr4

    # 3. Enable Long Mode
    mov $0xC0000080, %ecx # EFER MSR
    rdmsr
    or $0x100, %eax      # Set LME (Long Mode Enable) bit
    wrmsr

    # 4. Enable Paging
    mov %cr0, %eax
    or $0x80000001, %eax # Set PG (Paging) and PE (Protection Enable) bits
    mov %eax, %cr0

    # 5. Load the 64-bit GDT
    lgdt gdt64_pointer

    # 6. Perform a long jump to our 64-bit kernel code.
    # 0x28 is the offset of our 64-bit code segment in the GDT.
    # kmain64 is the label for our 64-bit entry point.
    ljmp $0x28, $kmain64

.section .text.kmain64
.global kmain64
.extern kmain_cpp # The final C++ entry point

.code64
kmain64:
    # We are now in 64-bit long mode, running at a high virtual address.
    # The first argument (multiboot info) is now in RDI.
    # We can now call our final C++ or high-level kernel main function.
    call kmain_cpp

.hang:
    cli
    hlt
    jmp .hang
