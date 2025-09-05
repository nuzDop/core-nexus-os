#include <stdint.h>

#define SYS_WRITE 15
#define SYS_READ  14
#define SYS_FORK  17
#define SYS_EXECVE 18
#define STDOUT 1
#define STDIN  0

int syscall(int num, int p1, int p2, int p3, int p4, int p5);
void strcpy(char* dest, const char* src);
int strlen(const char* str);

void _start() {
    char* prompt = "> ";
    char input_buf[128];

    while(1) {
        syscall(SYS_WRITE, STDOUT, (int)prompt, strlen(prompt), 0, 0);
        int bytes_read = syscall(SYS_READ, STDIN, (int)input_buf, 127, 0, 0);
        if (bytes_read > 0) {
            input_buf[bytes_read - 1] = '\0'; // Remove newline

            if (strlen(input_buf) == 0) continue;

            if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
                // Child process
                syscall(SYS_EXECVE, (int)input_buf, 0, 0, 0, 0);
                // If execve fails:
                char* err = "Command not found.\n";
                syscall(SYS_WRITE, STDOUT, (int)err, strlen(err), 0,0);
                // Exit syscall would be needed here
                while(1);
            } else {
                // Parent waits for child (waitpid syscall needed)
            }
        }
    }
}

// ... (strlen, strcpy implementations) ...
