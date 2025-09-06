;
; 64-bit Interrupt Service Routines and Context Switching
;

bits 64

; --- External C functions and variables ---
extern interrupt_handler_c
extern current_thread

; --- Macros for building ISRs ---
; Macro to build an ISR stub with no error code
%macro ISR_NO_ERR_CODE 1
global isr%1
isr%1:
    cli          ; Disable interrupts
    push 0       ; Push a dummy error code
    push %1      ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; Macro to build an ISR stub with a real error code pushed by the CPU
%macro ISR_ERR_CODE 1
global isr%1
isr%1:
    cli
    ; Error code is already on the stack
    push %1      ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; --- ISR and IRQ Definitions ---
; Define the first 32 ISRs for exceptions
ISR_NO_ERR_CODE 0
ISR_NO_ERR_CODE 1
ISR_NO_ERR_CODE 2
ISR_NO_ERR_CODE 3
ISR_NO_ERR_CODE 4
ISR_NO_ERR_CODE 5
ISR_NO_ERR_CODE 6
ISR_NO_ERR_CODE 7
ISR_ERR_CODE    8
ISR_NO_ERR_CODE 9
ISR_ERR_CODE    10
ISR_ERR_CODE    11
ISR_ERR_CODE    12
ISR_ERR_CODE    13
ISR_ERR_CODE    14
ISR_NO_ERR_CODE 15
ISR_NO_ERR_CODE 16
ISR_ERR_CODE    17
ISR_NO_ERR_CODE 18
ISR_NO_ERR_CODE 19
ISR_NO_ERR_CODE 20
ISR_ERR_CODE    21
; ... ISRs 22-31 are reserved ...
ISR_NO_ERR_CODE 31

; Define IRQs
global irq0, irq1, irq12, isr128
irq0:  ; Timer
    cli
    push 0
    push 32
    jmp isr_common_stub

irq1:  ; Keyboard
    cli
    push 0
    push 33
    jmp isr_common_stub
    
irq12: ; Mouse
    cli
    push 0
    push 44
    jmp isr_common_stub

isr128: ; Syscall
    cli
    push 0
    push 128
    jmp isr_common_stub


; --- Common ISR Stub ---
; This is where all ISRs and IRQs jump after pushing their specific info.
isr_common_stub:
    ; Save all general-purpose registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; The stack now holds the full register state.
    ; The C handler expects a pointer to this state in RDI.
    mov rdi, rsp
    
    ; Call the C-level interrupt handler
    call interrupt_handler_c

    ; Restore all general-purpose registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Pop the interrupt number and error code
    add rsp, 16

    sti          ; Re-enable interrupts
    iretq        ; Return from interrupt (64-bit)


; --- Context Switch Implementation ---
; void context_switch(registers_t* old, registers_t* new);
; RDI = old, RSI = new
global context_switch
context_switch:
    ; Save the state of the old thread
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    
    ; old->rsp = current stack pointer
    mov [rdi + 8*15], rsp

    ; Restore the state of the new thread
    ; new->rsp is the new stack pointer
    mov rsp, [rsi + 8*15]

    ; new->rip is the new instruction pointer. The 'ret' will jump to it.
    mov rax, [rsi + 8*16]
    push rax

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    
    ; Load the new page directory (PML4)
    mov rax, [current_thread]
    mov rax, [rax + 8] ; thread->parent_process
    mov rax, [rax + 8] ; process->pml4
    mov cr3, rax
    
    ret
