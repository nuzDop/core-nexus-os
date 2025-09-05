#include <stdint.h>

#define SYS_CREATE_WINDOW   4
#define SYS_POLL_EVENT      6
#define SYS_PRINT           1
#define SYS_PIPE_CREATE     28
#define SYS_DUP2            29
#define SYS_FORK            17
#define SYS_EXECVE          18
#define SYS_READ            14
#define SYS_WRITE           15
#define SYS_CLOSE           13

#define STDIN  0
#define STDOUT 1
#define STDERR 2

// Event struct definition...

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

void _start() {
    int win_id = syscall(SYS_CREATE_WINDOW, 200, 200, 640, 480, (int)"Terminal");
    
    int stdin_pipe[2];
    int stdout_pipe[2];

    syscall(SYS_PIPE_CREATE, (int)stdin_pipe, 0,0,0,0); // stdin_pipe[0] is read, [1] is write
    syscall(SYS_PIPE_CREATE, (int)stdout_pipe, 0,0,0,0);

    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        // Child process (will become the shell)
        syscall(SYS_CLOSE, stdin_pipe[1], 0,0,0,0); // Close write end of stdin pipe
        syscall(SYS_CLOSE, stdout_pipe[0], 0,0,0,0); // Close read end of stdout pipe

        syscall(SYS_DUP2, stdin_pipe[0], STDIN, 0,0,0);
        syscall(SYS_DUP2, stdout_pipe[1], STDOUT, 0,0,0);
        syscall(SYS_DUP2, stdout_pipe[1], STDERR, 0,0,0);

        char* path = "sh.elf";
        syscall(SYS_EXECVE, (int)path, 0, 0, 0, 0);
        while(1); // Should not be reached
    } else {
        // Parent process (the terminal)
        syscall(SYS_CLOSE, stdin_pipe[0], 0,0,0,0); // Close read end of stdin pipe
        syscall(SYS_CLOSE, stdout_pipe[1], 0,0,0,0); // Close write end of stdout pipe

        event_t event;
        char buffer[256];

        while(1) {
            // Check for keyboard input and send to shell
            if (syscall(SYS_POLL_EVENT, (int)&event, 0, 0, 0, 0)) {
                if (event.type == 5 /*EVENT_KEY_PRESS*/) {
                    char c = (char)event.data1;
                    syscall(SYS_WRITE, stdin_pipe[1], (int)&c, 1, 0, 0);
                }
            }

            // Check for output from shell and draw it
            int bytes_read = syscall(SYS_READ, stdout_pipe[0], (int)buffer, 255, 0, 0);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                // A real terminal would draw this text to its window buffer.
                // For now, we print to the debug console.
                syscall(SYS_PRINT, (int)buffer, 0,0,0,0);
            }
        }
    }
}
