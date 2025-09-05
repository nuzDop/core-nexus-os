#include "task.h"
#include "elf.h"
#include "../mem/pmm.h"
#include "../lib/string.h"

static thread_t* ready_queue_head = NULL;
static thread_t* ready_queue_tail = NULL;
static spinlock_t scheduler_lock = 0;
static int next_pid = 1;
static int next_tid = 1;

extern void context_switch(registers_t* old, registers_t* new);

void scheduler_init() {
    process_t* kernel_process = pmm_alloc_page();
    memset(kernel_process, 0, sizeof(process_t));
    kernel_process->pid = next_pid++;
    kernel_process->pml4 = (pml4_t*)current_pml4;

    thread_t* idle_thread = pmm_alloc_page();
    memset(idle_thread, 0, sizeof(thread_t));
    idle_thread->tid = next_tid++;
    idle_thread->parent_process = kernel_process;
    idle_thread->state = THREAD_RUNNING;
    
    current_thread = idle_thread;
}

void scheduler_add_thread(thread_t* thread) {
    spinlock_acquire(&scheduler_lock);
    thread->next = NULL;
    if (ready_queue_tail) {
        ready_queue_tail->next = thread;
        ready_queue_tail = thread;
    } else {
        ready_queue_head = ready_queue_tail = thread;
    }
    spinlock_release(&scheduler_lock);
}

void schedule() {
    spinlock_acquire(&scheduler_lock);

    thread_t* old_thread = current_thread;
    thread_t* next_thread = ready_queue_head;

    if (!next_thread) {
        spinlock_release(&scheduler_lock);
        return;
    }

    ready_queue_head = next_thread->next;
    if (!ready_queue_head) {
        ready_queue_tail = NULL;
    }

    if (old_thread->state == THREAD_RUNNING) {
        scheduler_add_thread(old_thread);
    }
    
    current_thread = next_thread;
    
    spinlock_release(&scheduler_lock);
    context_switch(&old_thread->regs, &next_thread->regs);
}

void thread_sleep_on(wait_queue_t** queue) {
    spinlock_acquire(&scheduler_lock);
    
    wait_queue_t wait_node;
    wait_node.waiting_thread = current_thread;
    wait_node.next = *queue;
    *queue = &wait_node;
    
    current_thread->state = THREAD_SLEEPING;
    
    spinlock_release(&scheduler_lock);
    
    schedule();
}

void thread_wakeup(wait_queue_t** queue) {
    spinlock_acquire(&scheduler_lock);
    
    wait_queue_t* current = *queue;
    while(current) {
        thread_t* thread = current->waiting_thread;
        thread->state = THREAD_RUNNING;
        scheduler_add_thread(thread);
        current = current->next;
    }
    *queue = NULL;

    spinlock_release(&scheduler_lock);
}

pid_t sys_fork(registers_t* parent_regs) {
    process_t* parent_proc = current_thread->parent_process;

    process_t* child_proc = pmm_alloc_page();
    memcpy(child_proc, parent_proc, sizeof(process_t));
    child_proc->pid = next_pid++;
    child_proc->parent = parent_proc;
    child_proc->pml4 = clone_pml4(parent_proc->pml4);

    thread_t* child_thread = pmm_alloc_page();
    memcpy(child_thread, current_thread, sizeof(thread_t));
    child_thread->tid = next_tid++;
    child_thread->parent_process = child_proc;
    
    child_thread->kernel_stack = (uint64_t)pmm_alloc_page() + KERNEL_STACK_SIZE;
    child_thread->regs = *parent_regs;
    child_thread->regs.rax = 0;
    
    scheduler_add_thread(child_thread);
    
    return child_proc->pid;
}

int sys_execve(const char* path, const char* const* argv, const char* const* envp) {
    return -1;
}

pid_t sys_waitpid(pid_t pid, int* status, int options) {
    return -1;
}
