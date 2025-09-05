/* user/terminal/src/main.c */

#include <stdint.h>
#include <stdbool.h>

// --- Syscall Definitions ---
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
#define SYS_YIELD           0
#define SYS_DRAW_RECT       5
// We need a syscall to draw a single character at a specific location
// #define SYS_DRAW_CHAR_IN_WINDOW 32

#define STDIN  0
#define STDOUT 1
#define STDERR 2

// --- Terminal Configuration ---
#define TERM_WIDTH 80  // Characters
#define TERM_HEIGHT 30 // Characters
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16
#define WINDOW_WIDTH (TERM_WIDTH * CHAR_WIDTH)
#define WINDOW_HEIGHT (TERM_HEIGHT * CHAR_HEIGHT)
#define COLOR_BG 0x080E1C
#define COLOR_FG 0xF9FAFB

// --- Event Structures ---
typedef enum {
    EVENT_NONE, EVENT_MOUSE_PRESS, EVENT_MOUSE_RELEASE, EVENT_MOUSE_MOVE,
    EVENT_KEY_PRESS, EVENT_WINDOW_CLOSE, EVENT_BUTTON_CLICK, EVENT_SYSTEM_ALERT
} event_type_t;

typedef struct {
    event_type_t type;
    int32_t data1; int32_t data2; int32_t data3;
} event_t;


int syscall(int num, int p1, int p2, int p3, int p4, int p5);

// --- Terminal State ---
char term_buffer[TERM_HEIGHT][TERM_WIDTH];
int cursor_x = 0;
int cursor_y = 0;
int win_id = -1;
bool needs_redraw = true;


void term_scroll() {
    // A real memmove syscall would be better
    for (int y = 0; y < TERM_HEIGHT - 1; y++) {
        for (int x = 0; x < TERM_WIDTH; x++) {
            term_buffer[y][x] = term_buffer[y + 1][x];
        }
    }
    // Clear the last line
    for (int x = 0; x < TERM_WIDTH; x++) {
        term_buffer[TERM_HEIGHT - 1][x] = ' ';
    }
}

void term_put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
        }
    } else {
        term_buffer[cursor_y][cursor_x] = c;
        cursor_x++;
        if (cursor_x >= TERM_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= TERM_HEIGHT) {
        term_scroll();
        cursor_y = TERM_HEIGHT - 1;
    }
    needs_redraw = true;
}

void term_redraw() {
    if (!needs_redraw) return;

    syscall(SYS_DRAW_RECT, win_id, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG, 0);
    char str[2] = {0};
    for (int y = 0; y < TERM_HEIGHT; y++) {
        for (int x = 0; x < TERM_WIDTH; x++) {
            if (term_buffer[y][x] != ' ') {
                str[0] = term_buffer[y][x];
                // syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)str, x * CHAR_WIDTH, y * CHAR_HEIGHT, COLOR_FG);
            }
        }
    }
    
    // Draw cursor
    // syscall(SYS_DRAW_RECT, win_id, cursor_x * CHAR_WIDTH, cursor_y * CHAR_HEIGHT, CHAR_WIDTH, CHAR_HEIGHT, COLOR_FG, 0);

    needs_redraw = false;
}

void _start() {
    win_id = syscall(SYS_CREATE_WINDOW, 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, (int)"Terminal");
    
    int stdin_pipe_fds[2];  // [0] = read, [1] = write
    int stdout_pipe_fds[2]; // [0] = read, [1] = write
    syscall(SYS_PIPE_CREATE, (int)stdin_pipe_fds, 0, 0, 0, 0);
    syscall(SYS_PIPE_CREATE, (int)stdout_pipe_fds, 0, 0, 0, 0);

    int pid = syscall(SYS_FORK, 0, 0, 0, 0, 0);

    if (pid == 0) {
        // --- Child Process (Shell) ---
        // Close unused pipe ends
        syscall(SYS_CLOSE, stdin_pipe_fds[1], 0, 0, 0, 0);  // Close write end of our stdin
        syscall(SYS_CLOSE, stdout_pipe_fds[0], 0, 0, 0, 0); // Close read end of our stdout

        // Redirect standard I/O
        syscall(SYS_DUP2, stdin_pipe_fds[0], STDIN, 0, 0, 0);
        syscall(SYS_DUP2, stdout_pipe_fds[1], STDOUT, 0, 0, 0);
        syscall(SYS_DUP2, stdout_pipe_fds[1], STDERR, 0, 0, 0);

        char* path = "sh.elf";
        syscall(SYS_EXECVE, (int)path, 0, 0, 0, 0);
        while(1); // Should not be reached
    } else {
        // --- Parent Process (Terminal) ---
        // Close unused pipe ends
        syscall(SYS_CLOSE, stdin_pipe_fds[0], 0, 0, 0, 0);  // Close read end of shell's stdin
        syscall(SYS_CLOSE, stdout_pipe_fds[1], 0, 0, 0, 0); // Close write end of shell's stdout

        event_t event;
        char read_buf[256];

        while(1) {
            // 1. Handle user input
            if (syscall(SYS_POLL_EVENT, (int)&event, 0, 0, 0, 0)) {
                if (event.type == EVENT_KEY_PRESS) {
                    char c = (char)event.data1;
                    syscall(SYS_WRITE, stdin_pipe_fds[1], (int)&c, 1, 0, 0);
                }
            }

            // 2. Read output from shell
            int bytes_read = syscall(SYS_READ, stdout_pipe_fds[0], (int)read_buf, 255, 0, 0);
            if (bytes_read > 0) {
                for (int i = 0; i < bytes_read; i++) {
                    term_put_char(read_buf[i]);
                }
            }
            
            // 3. Redraw the terminal window if needed
            term_redraw();
            syscall(SYS_YIELD, 0,0,0,0,0);
        }
    }
}
