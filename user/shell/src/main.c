#include <stdint.h>

#define SYS_PRINT           1
#define SYS_CREATE_WINDOW   4
#define SYS_DRAW_RECT       5
#define SYS_FORK            17
#define SYS_EXECVE          18
#define SYS_POLL_EVENT      6

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

typedef struct {
    int type;
    int data1;
    int data2;
    int data3;
} event_t;

void _start() {
    syscall(SYS_CREATE_WINDOW, 0, 0, 1024, 32, (int)"Taskbar");
    
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "guitest.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }

    event_t event;
    while(1) {
        // The shell can now poll for events, e.g., to handle a start menu click
        syscall(SYS_POLL_EVENT, (int)&event, 0,0,0,0);
    }
}
