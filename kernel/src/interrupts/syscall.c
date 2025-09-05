#include "syscall.h"
#include "../fs/vfs.h"
#include "../lib/string.h"
#include "../gui/compositor.h"
#include "../gui/window.h"
#include "../../../arch/x86_64/gdt.h"
#include "../gui/events.h"
#include "../ai/nexus_core.h"
#include "../net/sockets.h"
#include <stddef.h>

void print(char*);
void* memcpy(void* dest, const void* src, size_t n);
int fork(void);
int execve(const char *path, char **argv, char **envp);

typedef void (*syscall_handler_t)(registers_t* regs);
syscall_handler_t syscall_handlers[20];

void sys_yield_handler(registers_t* regs) { (void)regs; switch_task(); }
void sys_print_handler(registers_t* regs) { print((char*)regs->ebx); }
// ... other existing handlers ...

// VFS Syscalls (Placeholders for now)
void sys_open_handler(registers_t* regs) { regs->eax = -1; }
void sys_close_handler(registers_t* regs) { regs->eax = -1; }
void sys_read_handler(registers_t* regs) { regs->eax = -1; }
void sys_write_handler(registers_t* regs) {
    // Simple version for stdout/stderr
    if (regs->ebx == 1 || regs->ebx == 2) {
        print((char*)regs->ecx);
        regs->eax = strlen((char*)regs->ecx);
    }
}
void sys_stat_handler(registers_t* regs) { regs->eax = -1; }

// Process Syscalls
void sys_fork_handler(registers_t* regs) { regs->eax = fork(); }
void sys_execve_handler(registers_t* regs) {
    regs->eax = execve((const char*)regs->ebx, (char**)regs->ecx, (char**)regs->edx);
}
void sys_waitpid_handler(registers_t* regs) { regs->eax = -1; }

void syscall_dispatcher(registers_t* regs) {
    nexus_core_analyze_syscall(regs);
    if (regs->eax < 20 && syscall_handlers[regs->eax]) {
        syscall_handler_t handler = syscall_handlers[regs->eax];
        handler(regs);
    }
}

// Add handlers for new TCP socket syscalls
void sys_connect_handler(registers_t* regs) { /* ... */ }
void sys_listen_handler(registers_t* regs) { /* ... */ }
void sys_accept_handler(registers_t* regs) { /* ... */ }
void sys_send_handler(registers_t* regs) { /* ... */ }
void sys_recv_handler(registers_t* regs) { /* ... */ }

void sys_poll_event_handler(registers_t* regs) {
    event_queue_t* q = &current_task->event_queue;
    if (q->head == q->tail) {
        regs->eax = 0; // No event
        return;
    }
    // Copy event to user-space buffer
    memcpy((void*)regs->ebx, &q->events[q->tail], sizeof(event_t));
    q->tail = (q->tail + 1) % EVENT_QUEUE_SIZE;
    regs->eax = 1; // Event was polled
}

void init_syscalls() {
    // ...
    syscall_handlers[SYS_CONNECT] = &sys_connect_handler;
    syscall_handlers[SYS_LISTEN] = &sys_listen_handler;
    syscall_handlers[SYS_ACCEPT] = &sys_accept_handler;
    syscall_handlers[SYS_SEND] = &sys_send_handler;
    syscall_handlers[SYS_RECV] = &sys_recv_handler;
    syscall_handlers[SYS_YIELD] = &sys_yield_handler;
    syscall_handlers[SYS_PRINT] = &sys_print_handler;
    // ...
    syscall_handlers[SYS_OPEN] = &sys_open_handler;
    syscall_handlers[SYS_CLOSE] = &sys_close_handler;
    syscall_handlers[SYS_READ] = &sys_read_handler;
    syscall_handlers[SYS_WRITE] = &sys_write_handler;
    syscall_handlers[SYS_STAT] = &sys_stat_handler;
    syscall_handlers[SYS_FORK] = &sys_fork_handler;
    syscall_handlers[SYS_EXECVE] = &sys_execve_handler;
    syscall_handlers[SYS_WAITPID] = &sys_waitpid_handler;
}

void sys_create_widget_handler(registers_t* regs) {
    int win_id = regs->ebx;
    // Find window by ID
    // ...
    widget_t* widget = create_widget((widget_type_t)regs->ecx, NULL, regs->edx, regs->esi, regs->edi, regs->ebp, (const char*)regs->eax);
    regs->eax = widget->id;
}


void init_syscalls() {
    // ...
    syscall_handlers[SYS_CREATE_WIDGET] = &sys_create_widget_handler;
}
