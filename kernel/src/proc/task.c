#include "task.h"
#include "../mem/pmm.h"
#include "elf.h"
#include "../../../arch/x86_64/gdt.h"

volatile task_t* current_task;
// One ready queue for each priority level
volatile task_t* ready_queues[NUM_PRIORITIES]; 
uint32_t next_pid = 1;

void print(char*);

// Assembly function to perform the context switch
extern void perform_task_switch(uint32_t eip, uint32_t esp, uint32_t ebp, uint32_t cr3);

void init_tasking() {
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        ready_queues[i] = 0;
    }

    current_task = (task_t*)pmm_alloc_page();
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = kernel_directory;
    current_task->priority = PRIORITY_NORMAL;
    current_task->ticks_left = 10; // Default time slice
    current_task->next = 0;
    
    // Add the initial kernel task to the normal priority queue
    ready_queues[PRIORITY_NORMAL] = current_task;
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

// Multi-Level Feedback Queue Scheduler
void switch_task() {
    if (!current_task) return;

    current_task->ticks_left--;

    if (current_task->ticks_left <= 0) {
        // Task used its full time slice, demote its priority
        if (current_task->priority < PRIORITY_LOW) {
            current_task->priority++;
        }
        // Reset ticks for the new priority level
        current_task->ticks_left = 10 * (current_task->priority + 1);
    }

    // Find the next task to run, starting from the highest priority queue
    for (int i = 0; i < NUM_PRIORITIES; i++) {
        if (ready_queues[i]) {
            task_t* next = (task_t*)ready_queues[i];
            // Simple round-robin within a priority level
            ready_queues[i] = next->next;
            next->next = NULL;

            // Move the current task to the end of its priority queue
            if (current_task) {
                 task_t* tail = (task_t*)ready_queues[current_task->priority];
                 if (!tail) {
                     ready_queues[current_task->priority] = current_task;
                 } else {
                     while(tail->next) tail = tail->next;
                     tail->next = current_task;
                 }
            }

            current_task = next;
            perform_task_switch(current_task->eip, current_task->esp, current_task->ebp, current_task->page_directory->physical_addr);
            return;
        }
    }
}
