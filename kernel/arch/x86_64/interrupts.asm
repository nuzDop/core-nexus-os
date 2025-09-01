bits 32

global gdt_flush, idt_flush
global isr0, irq0, irq1, irq12, isr128
global perform_task_switch

extern interrupt_handler_c 

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret

isr0:
    cli
    push 0
    push 0
    jmp isr_common_stub

irq0:
    cli
    push 0
    push 32
    jmp isr_common_stub

irq1:
    cli
    push 0
    push 33
    jmp isr_common_stub

irq12:
    cli
    push 0
    push 44
    jmp isr_common_stub

isr128:
    cli
    push 0
    push 128
    jmp isr_common_stub

isr_common_stub:
    pusha
    mov ax, ds
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, [current_directory]
    mov eax, [eax]
    cmp eax, cr3
    je .no_dir_switch
    mov cr3, eax
.no_dir_switch:
    
    push esp
    call interrupt_handler_c
    pop esp

    pop eax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    popa
    add esp, 8
    sti
    iretd

perform_task_switch:
    mov eax, [esp + 4]
    mov esp, [esp + 8]
    mov ebp, [esp + 12]
    mov ecx, [esp + 16]
    mov cr3, ecx
    jmp eax
