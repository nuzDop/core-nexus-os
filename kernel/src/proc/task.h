#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stdbool.h>
#include "../mem/vmm.h"
#include "../fs/vfs.h"
#include "../sync/spinlock.h"

// Define standard types that were missing
typedef int pid_t;
#define MAX_FILES 16

#define KERNEL_STACK_SIZE 8192

typedef enum {
    THREAD_RUNNING,
    THREAD_SLEEPING,
    THREAD_ZOMBIE
} thread_state_t;

typedef struct registers {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) registers_t;

struct thread;

typedef struct wait_queue {
    struct thread* waiting_thread;
    struct wait_queue* next;
} wait_queue_t;

typedef struct process {
    pid_t pid;
    pml4_t* pml4;
    fs_node_t* file_descriptors[MAX_FILES];
    struct process* parent;
} process_t;

typedef struct thread {
    int tid;
    process_t* parent_process;
    thread_state_t state;
    uint64_t kernel_stack;
    registers_t regs;
    int exit_code;
    wait_queue_t* join_queue;
    struct thread* next;
} thread_t;

void scheduler_init();
void schedule();
void scheduler_add_thread(thread_t* thread);
void thread_sleep_on(wait_queue_t** queue);
void thread_wakeup(wait_queue_t** queue);
pid_t sys_fork(registers_t* regs);
int sys_execve(const char* path, const char* const* argv, const char* const* envp);
pid_t sys_waitpid(pid_t pid, int* status, int options);

// Make the current thread globally accessible
extern thread_t* current_thread;

#endif
