; LimitlessOS x86_64 Bootloader
; This file sets up the initial state for the PolyCore kernel.

bits 32 ; We start in 32-bit protected mode from the bootloader (e.g., GRUB)

section .multiboot
align 4
    ; Multiboot header constants for GRUB compatibility
    MULTIBOOT_HEADER_MAGIC      equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS      equ 0x00
    MULTIBOOT_HEADER_CHECKSUM   equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

    ; The multiboot header itself
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_HEADER_CHECKSUM

section .text
global start
extern kmain ; kmain is our entry point in main.c

start:
    ; At this point, the bootloader (like GRUB) has loaded us into memory
    ; and we are in 32-bit protected mode. The stack is already set up.

    ; Call the C part of our kernel.
    call kmain

    ; If kmain ever returns (it shouldn't), halt the CPU.
    cli
.hang:
    hlt
    jmp .hang
