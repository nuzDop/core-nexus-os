#ifndef PROC_TASK_H
#define PROC_TASK_H
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*

This header may be pulled in indirectly by fs/vfs.h via security/mac.h.

We forward-declare fs_node_t to avoid cyclic includes.
*/
struct fs_node;
typedef struct fs_node fs_node_t;

/* -------- System Limits (override elsewhere if needed) -------- */
#ifndef MAX_FILES
#define MAX_FILES 128
#endif

/* -------- Task/Thread States -------- */
typedef enum task_state {
TASK_STATE_UNUSED = 0,
TASK_STATE_READY,
TASK_STATE_RUNNING,
TASK_STATE_BLOCKED,
TASK_STATE_SLEEPING,
TASK_STATE_ZOMBIE,
TASK_STATE_STOPPED
} task_state_t;

/*

-------- Minimal CPU context (x86_64) saved on switch --------

Note: avoid field names like cs/ss that may collide with macros.
/
typedef struct cpu_context {
/ Callee-saved first (common convention for ASM switchers) */
uint64_t r15;
uint64_t r14;
uint64_t r13;
uint64_t r12;
uint64_t rbx;
uint64_t rbp;

/* Return path */
uint64_t rip;
uint64_t rflags;
uint64_t rsp;

/* Segment selectors (renamed to avoid macro collisions) */
uint64_t seg_cs;
uint64_t seg_ss;
} cpu_context_t;

/* -------- Credentials / Security (opaque pointer for MAC/etc) -------- /
typedef struct task_cred {
uint32_t uid;
uint32_t gid;
uint32_t euid;
uint32_t egid;
void security_label; / opaque (e.g., mac_label_t) */
} task_cred_t;

/* -------- Task control block -------- /
typedef struct task {
/ Identity */
int32_t pid;
int32_t ppid;
char name[32];
/* State / scheduling */
task_state_t state;
uint32_t     timeslice_ticks;
uint32_t     cpu_affinity;    /* preferred CPU id or mask */
uint32_t     cpu_running_on;  /* last CPU that ran this task */

/* Address space (arch-specific; e.g., PML4 for x86_64) */
void        *mm_root;

/* Kernel stack */
void        *kstack_base;
size_t       kstack_size;

/* Working directory and open files */
fs_node_t   *cwd;
fs_node_t   *file_descriptors[MAX_FILES];

/* Blocking/wait state */
void        *wait_obj;        /* channel/condvar/semaphore pointer */
uint32_t     wait_reason;     /* code describing why blocked */

/* Exit info */
int          exit_code;

/* Credentials */
task_cred_t  cred;

/* Saved CPU context for scheduler */
cpu_context_t ctx;

/* Scheduler links (opaque to avoid pulling list headers) */
void        *runq_prev;
void        *runq_next;
} task_t;

/* Current task pointer (provided by scheduler core) */
extern task_t *current_task;

#endif /* PROC_TASK_H */
