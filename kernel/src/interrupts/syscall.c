#include "syscall.h"
#include "../fs/vfs.h"
#include "../lib/string.h"
#include "../gui/window.h"
#include "../../../arch/x86_64/gdt.h"
#include "../gui/events.h"
#include "../ai/nexus_core.h"
#include "../net/sockets.h"
#include <stddef.h>

void print(char*);
void* memcpy(void* dest, const void* src, size_t n);

typedef void (*syscall_handler_t)(registers_t* regs);
syscall_handler_t syscall_handlers[11];

void sys_yield_handler(registers_t* regs) { (void)regs; switch_task(); }
void sys_print_handler(registers_t* regs) { print((char*)regs->ebx); }
void sys_create_handler(registers_t* regs) { fs_root->create(fs_root, (char*)regs->ebx, FS_FILE); }
void sys_write_handler(registers_t* regs) {
    fs_node_t* file = fs_root->finddir(fs_root, (char*)regs->ebx);
    if (file) file->write(file, 0, regs->edx, (uint8_t*)regs->ecx);
}
void sys_create_window_handler(registers_t* regs) {
    window_t* new_win = create_window(50, 50, (int)regs->ebx, (int)regs->ecx, (char*)regs->edx);
    regs->eax = new_win ? new_win->id : -1;
}
void sys_draw_rect_handler(registers_t* regs) {
    wm_draw_rect_in_window((int)regs->ebx, (int)regs->ecx, (int)regs->edx, (int)regs->esi, (int)regs->edi, (uint32_t)regs->ebp);
}
void sys_poll_event_handler(registers_t* regs) {
    event_queue_t* q = &current_task->event_queue;
    if (q->head == q->tail) { regs->eax = 0; return; }
    memcpy((void*)regs->ebx, &q->events[q->tail], sizeof(event_t));
    q->tail = (q->tail + 1) % EVENT_QUEUE_SIZE;
    regs->eax = 1;
}
void sys_socket_handler(registers_t* regs) {
    regs->eax = sys_socket(regs->ebx, regs->ecx, regs->edx);
}
void sys_bind_handler(registers_t* regs) {
    regs->eax = sys_bind(regs->ebx, (const sockaddr_in_t*)regs->ecx, regs->edx);
}
void sys_sendto_handler(registers_t* regs) {
    regs->eax = sys_sendto(regs->ebx, (const void*)regs->ecx, regs->edx, regs->esi, (const sockaddr_in_t*)regs->edi, (uint32_t)regs->ebp);
}
void sys_recvfrom_handler(registers_t* regs) {
    regs->eax = sys_recvfrom(regs->ebx, (void*)regs->ecx, regs->edx, regs->esi, (sockaddr_in_t*)regs->edi, (uint32_t*)regs->ebp);
}

void syscall_dispatcher(registers_t* regs) {
    nexus_core_analyze_syscall(regs);
    if (regs->eax < 11) {
        syscall_handler_t handler = syscall_handlers[regs->eax];
        if (handler) handler(regs);
    }
}

void init_syscalls() {
    syscall_handlers[SYS_YIELD] = &sys_yield_handler;
    syscall_handlers[SYS_PRINT] = &sys_print_handler;
    syscall_handlers[SYS_CREATE] = &sys_create_handler;
    syscall_handlers[SYS_WRITE] = &sys_write_handler;
    syscall_handlers[SYS_CREATE_WINDOW] = &sys_create_window_handler;
    syscall_handlers[SYS_DRAW_RECT] = &sys_draw_rect_handler;
    syscall_handlers[SYS_POLL_EVENT] = &sys_poll_event_handler;
    syscall_handlers[SYS_SOCKET] = &sys_socket_handler;
    syscall_handlers[SYS_BIND] = &sys_bind_handler;
    syscall_handlers[SYS_SENDTO] = &sys_sendto_handler;
    syscall_handlers[SYS_RECVFROM] = &sys_recvfrom_handler;
}
