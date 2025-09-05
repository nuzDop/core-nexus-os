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
#define SYS_GET_WINDOW_BUFFER 11
#define SYS_OPEN            12
#define SYS_CLOSE           13
#define SYS_READ            14
#define SYS_STAT            16
#define SYS_FORK            17
#define SYS_EXECVE          18
#define SYS_WAITPID         19
#define SYS_CONNECT         20
#define SYS_LISTEN          21
#define SYS_ACCEPT          22
#define SYS_SEND            23
#define SYS_RECV            24
#define SYS_CREATE_WIDGET   25
#define SYS_GET_SYSTEM_TIME 26
#define SYS_READDIR         27
#define SYS_PIPE_CREATE     28
#define SYS_DUP2            29
#define SYS_DRAW_STRING_IN_WINDOW 30
#define SYS_DRAW_ICON_IN_WINDOW   31
#define SYS_CLOSE           13

void init_syscalls();

#endif
