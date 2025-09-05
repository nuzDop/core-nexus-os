#include "task.h"
#include "elf.h"
#include "../mem/pmm.h"
#include "../mem/vmm.h"
#include "../lib/string.h"
#include "../../../arch/x86_64/gdt.h"

volatile task_t* current_task;
volatile task_t* ready_queue;
uint32_t next_pid = 1;

extern void perform_task_switch(uint32_t eip, uint32_t esp, uint32_t ebp, uint32_t cr3);

void init_tasking() {
    current_task = (task_t*)pmm_alloc_page();
    current_task->id = next_pid++;
    current_task->esp = current_task->ebp = 0;
    current_task->eip = 0;
    current_task->page_directory = kernel_directory;
    current_task->runtime = 0;
    current_task->timeslice = TIMESLICE_BASE;
    current_task->priority = 0;
    current_task->state = TASK_RUNNING;
    current_task->parent = NULL;
    current_task->next = 0;
    
    ready_queue = current_task;
}

void enter_user_mode(uint32_t entry_point, uint32_t stack) {
    __asm__ __volatile__(
        "cli;"
        "mov $0x23, %ax;"
        "mov %ax, %ds;"
        "mov %ax, %es;"
        "mov %ax, %fs;"
        "mov %ax, %gs;"
        "mov %0, %esp;" // Set user stack
        "pushl $0x23;"
        "pushl %0;"     // Push user stack pointer
        "pushf;"
        "popl %eax;"
        "orl $0x200, %eax;"
        "pushl %eax;"
        "pushl $0x1B;"
        "pushl %1;"     // Push entry point
        "iretd;"
        : : "r"(stack), "r"(entry_point) : "eax"
    );
}

void switch_task() {
    // ... implementation from previous step ...
}

void push_event_to_task(task_t* task, event_t event) {
    if (!task) return;
    uint32_t next_head = (task->event_queue.head + 1) % EVENT_QUEUE_SIZE;
    if (next_head == task->event_queue.tail) {
        return; // Event queue is full, drop event
    }
    task->event_queue.events[task->event_queue.head] = event;
    task->event_queue.head = next_head;
}

task_t* find_task_by_id(int id) {
    task_t* task = (task_t*)ready_queue;
    while(task) {
        if (task->id == id) return task;
        task = task->next;
    }
    return NULL;
}

int fork() {
    task_t* parent_task = (task_t*)current_task;
    task_t* child_task = (task_t*)pmm_alloc_page();
    
    memcpy(child_task, parent_task, sizeof(task_t));
    child_task->id = next_pid++;
    child_task->parent = parent_task;

    child_task->page_directory = clone_directory(parent_task->page_directory, true);

    // This is the magic that makes fork work. The child will start executing
    // from the return of this syscall, but with a different EAX value.
    // The EIP is set to the instruction after the syscall in the parent.
    // We need to get the parent's registers, which were saved on syscall entry.
    // Let's assume they are accessible. For now, this is a simplified model.
    child_task->eip = parent_task->eip; // Should be the saved EIP
    child_task->esp = parent_task->esp; // Should be the saved ESP
    child_task->ebp = parent_task->ebp; // Should be the saved EBP

    // Add to ready queue
    child_task->next = (struct task*)ready_queue;
    ready_queue = child_task;

    return child_task->id; // Parent returns child's PID
}

int execve(const char *path, char **argv, char **envp) {
    page_directory_t* new_dir = clone_directory(kernel_directory, false);
    uint32_t entry_point = elf_load((char*)path, new_dir);

    if (entry_point == 0) {
        // Free new_dir memory
        return -1;
    }

    // Free the old address space (except kernel parts)
    // ... (implementation needed) ...
    
    current_task->page_directory = new_dir;
    switch_page_directory(new_dir);

    // Create a new user stack
    uint32_t user_stack_top = 0xC0000000;
    uint32_t user_stack_bottom = user_stack_top - PAGE_SIZE;
    map_page(user_stack_bottom, (uint32_t)pmm_alloc_page(), new_dir);

    // Push argv, envp onto the new stack... (complex part, simplified for now)

    enter_user_mode(entry_point, user_stack_top);

    return 0; // Should not be reached
}
