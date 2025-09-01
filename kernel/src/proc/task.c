#include "task.h"
#include "../mem/pmm.h"
#include "elf.h"
#include "../../../arch/x86_64/gdt.h"

volatile task_t* current_task;
volatile task_t* ready_queue;
uint32_t next_pid = 1;

void print(char*);

// Assembly function to perform the context switch
extern void perform_task_switch(uint32_t eip, uint32_t esp, uint32_t ebp, uint32_t cr3);

void init_tasking() {
    current_task = (task_t*)pmm_alloc_page();
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = kernel_directory;
    current_task->next = 0;
    ready_queue = current_task;
}

void enter_user_mode(uint32_t entry_point) {
    // This function sets up the stack to trick the CPU into returning to user-mode.
    // The iretd instruction will pop these values into the registers.
    // 0x23 is the user data segment selector, 0x1B is the user code segment selector.
    __asm__ __volatile__(
        "cli;"
        "mov $0x23, %ax;"
        "mov %ax, %ds;"
        "mov %ax, %es;"
        "mov %ax, %fs;"
        "mov %ax, %gs;"
        "mov %esp, %eax;"
        "pushl $0x23;"         // User stack segment
        "pushl %eax;"          // User stack pointer
        "pushf;"               // Push EFLAGS
        "popl %eax;"           // Pop EFLAGS
        "orl $0x200, %eax;"    // Set the IF bit (enable interrupts)
        "pushl %eax;"          // Push modified EFLAGS
        "pushl $0x1B;"         // User code segment
        "pushl %0;"            // Entry point EIP
        "iretd;"
        : : "r"(entry_point) : "eax"
    );
}

void switch_task() {
    if (!current_task) return;
    
    // This is a placeholder for saving the current task's state (EIP, ESP, EBP)
    // A real implementation would get these values from the stack.
    
    current_task = current_task->next ? current_task->next : ready_queue;
    
    // Switch to the next task's memory space and jump to its instruction pointer.
    // The values passed here are the *kernel* state of the next task.
    perform_task_switch(current_task->eip, current_task->esp, current_task->ebp, current_task->page_directory->physical_addr);
}
