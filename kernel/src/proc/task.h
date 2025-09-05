#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include "../mem/vmm.h"
#include "../gui/events.h" // Include event queue definition

// Task priority levels for the scheduler
#define PRIORITY_HIGH   0
#define PRIORITY_NORMAL 1
#define PRIORITY_LOW    2
#define NUM_PRIORITIES  3

typedef struct registers {
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags, useresp, ss;
} registers_t;

typedef struct task {
    int id;
    uint32_t esp, ebp;
    uint32_t eip;
    page_directory_t* page_directory;
    uint32_t kernel_stack;
    event_queue_t event_queue;
    
    // Scheduler-specific fields
    int priority;
    int ticks_left;

    struct task* next;
} task_t;

void init_tasking();
void switch_task();
void enter_user_mode(uint32_t entry_point);

extern volatile task_t* current_task;
// Queues for multi-level feedback scheduler
extern volatile task_t* ready_queues[NUM_PRIORITIES];

#endif
