#include "syscall.h"
#include "../drivers/cmos.h"
#include "../fs/vfs.h"
#include "../lib/string.h"
#include "../gui/compositor.h"
#include "../gui/window.h"
#include "../../../arch/x86_64/gdt.h"
#include "../gui/events.h"
#include "../ai/nexus_core.h"
#include "../net/sockets.h"
#include "../ipc/pipe.h"
#include <stddef.h>
#include "../gui/icons.h"

void sys_draw_string_in_window_handler(registers_t* regs) {
    int win_id = regs->ebx;
    const char* text = (const char*)regs->ecx;
    int x = regs->edx;
    int y = regs->esi;
    uint32_t color = regs->edi;
    // Find window by ID, then call draw_string on its buffer
}

void sys_draw_icon_in_window_handler(registers_t* regs) {
    int win_id = regs->ebx;
    icon_type_t icon = (icon_type_t)regs->ecx;
    int x = regs->edx;
    int y = regs->esi;
    // Find window by ID, then draw the specified icon to its buffer
}

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
void sys_pipe_create_handler(registers_t* regs) {
    regs->eax = create_pipe((int*)regs->ebx);
}

void sys_dup2_handler(registers_t* regs) {
    int old_fd = regs->ebx;
    int new_fd = regs->ecx;
    if (old_fd >= MAX_FILES || new_fd >= MAX_FILES || !current_task->file_descriptors[old_fd]) {
        regs->eax = -1;
        return;
    }
    current_task->file_descriptors[new_fd] = current_task->file_descriptors[old_fd];
    regs->eax = new_fd;
}

void sys_open_handler(registers_t* regs) { regs->eax = -1; }

void sys_close_handler(registers_t* regs) {
    int fd = regs->ebx;
    if (fd < MAX_FILES && current_task->file_descriptors[fd]) {
        close_fs(current_task->file_descriptors[fd]);
        current_task->file_descriptors[fd] = NULL;
        regs->eax = 0;
    } else {
        regs->eax = -1;
    }
}

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

void sys_get_system_time_handler(registers_t* regs) {
    rtc_time_t* time_buf = (rtc_time_t*)regs->ebx;
    if (time_buf) {
        get_rtc_time(time_buf);
    }
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
    // ...
    syscall_handlers[SYS_GET_SYSTEM_TIME] = &sys_get_system_time_handler;
    // ...
    syscall_handlers[SYS_PIPE_CREATE] = &sys_pipe_create_handler;
    syscall_handlers[SYS_DUP2] = &sys_dup2_handler;
    // ...
    syscall_handlers[SYS_DRAW_STRING_IN_WINDOW] = &sys_draw_string_in_window_handler;
    syscall_handlers[SYS_DRAW_ICON_IN_WINDOW] = &sys_draw_icon_in_window_handler;
}

void sys_create_widget_handler(registers_t* regs) {
    int win_id = regs->ebx;
    // Find window by ID
    // ...
    widget_t* widget = create_widget((widget_type_t)regs->ecx, NULL, regs->edx, regs->esi, regs->edi, regs->ebp, (const char*)regs->eax);
    regs->eax = widget->id;
}

void sys_readdir_handler(registers_t* regs) {
    fs_node_t* node = finddir_fs(fs_root, (char*)regs->ebx);
    if (node) {
        struct dirent* de = readdir_fs(node, regs->ecx);
        if (de) {
            memcpy((void*)regs->edx, de, sizeof(struct dirent));
            regs->eax = 1;
        } else {
            regs->eax = 0;
        }
    } else {
        regs->eax = 0;
    }
}

void init_syscalls() {
    // ...
    syscall_handlers[SYS_CREATE_WIDGET] = &sys_create_widget_handler;

    syscall_handlers[SYS_READDIR] = &sys_readdir_handler;
}
