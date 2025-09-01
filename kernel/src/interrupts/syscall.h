#ifndef SYSCALL_H
#define SYSCALL_H

#include "../proc/task.h"

#define SYS_YIELD           0
#define SYS_PRINT           1
#define SYS_CREATE          2
#define SYS_WRITE           3
#define SYS_CREATE_WINDOW   4
#define SYS_DRAW_RECT       5
#define SYS_POLL_EVENT      6
#define SYS_SOCKET          7
#define SYS_BIND            8
#define SYS_SENDTO          9
#define SYS_RECVFROM        10

void init_syscalls();

#endif
