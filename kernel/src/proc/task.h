#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "../mem/vmm.h"
#include "../gui/events.h"
#include "../fs/vfs.h"
#include "../security/mac.h"

#define STDIN  0
#define STDOUT 1
#define STDERR 2
#define TIMESLICE_BASE 10
#define MAX_FILES 16

typedef enum {
    TASK_RUNNING,
    TASK_SLEEPING,
    TASK_ZOMBIE
} task_state_t;

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

    int64_t runtime;
    int timeslice;
    int priority;
    task_state_t state;
    int exit_code;
    
    fs_node_t* file_descriptors[MAX_FILES];

    security_context_t sec_ctx;

    struct task* next;
    struct task* parent;
} task_t;

void init_tasking();
void switch_task();
void enter_user_mode(uint32_t entry_point, uint32_t stack);

void push_event_to_task(task_t* task, event_t event);
task_t* find_task_by_id(int id);

int fork(void);
int execve(const char *path, char **argv, char **envp);

extern volatile task_t* current_task;
extern volatile task_t* ready_queue;
extern uint32_t next_pid;

#endif
