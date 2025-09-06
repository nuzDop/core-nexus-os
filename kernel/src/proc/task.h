#ifndef __KERNEL_PROC_TASK_H__
#define __KERNEL_PROC_TASK_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <mem/vmm.h>
#include <fs/vfs.h>
#include <sync/spinlock.h>

#define MAX_TASKS 1024
#define MAX_FILES_PER_TASK 256
#define KERNEL_STACK_SIZE 0x4000 // 16 KB

typedef int pid_t;

// Represents the state of the CPU registers.
// This structure is architecture-specific (x86_64).
typedef struct {
    uint64_t rax, rbx, rcx, rdx, rsi, rdi, rbp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;
    uint64_t rip, rflags, rsp;
    uint64_t cs, ss;
} cpu_context_t;

// Security credentials for a task.
typedef struct {
    uint32_t uid; // User ID
    uint32_t gid; // Group ID
    uint32_t euid; // Effective User ID
    uint32_t egid; // Effective Group ID
} task_cred_t;

// Task state
typedef enum {
    TASK_STATE_RUNNING,
    TASK_STATE_READY,
    TASK_STATE_SLEEPING,
    TASK_STATE_ZOMBIE,
    TASK_STATE_DEAD
} task_state_t;

// Task structure
typedef struct task {
    pid_t id;
    task_state_t state;
    
    cpu_context_t context;
    
    pagemap_t* pagemap; // Virtual address space
    
    uintptr_t kernel_stack;
    uintptr_t user_stack;

    task_cred_t cred;

    // File descriptors
    vfs_node_t* file_descriptors[MAX_FILES_PER_TASK];

    // Parent/child relationship
    struct task* parent;
    
    // Linked list of tasks
    struct task* next;

    // Synchronization
    spinlock_t lock;

    // Exit code for zombie tasks
    int exit_code;

} task_t;

extern task_t* current_task;

// Function prototypes
void task_init(void);
task_t* create_task(const char* name, void (*entry)(void), bool is_kernel_task);
void switch_to_task(task_t* task);
void schedule(void);

#endif // __KERNEL_PROC_TASK_H__
