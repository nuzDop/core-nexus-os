#include <stdint.h>

#define SYS_PRINT           1
#define SYS_CREATE_WINDOW   4
#define SYS_DRAW_RECT       5
#define SYS_FORK            17
#define SYS_EXECVE          18
#define SYS_POLL_EVENT      6

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

#define SYS_GET_SYSTEM_TIME 26

#define COLOR_TASKBAR 0x101827
#define COLOR_FOREGROUND 0xF9FAFB

typedef struct {
    uint8_t second; uint8_t minute; uint8_t hour;
    uint8_t day; uint8_t month; uint16_t year;
} rtc_time_t;

void itoa(int n, char s[], int base);

void _start() {
    // The shell now creates a full-screen, click-through window for the background
    // and a separate, smaller window for the taskbar.
    // This is simplified for now to a single taskbar window.
    int taskbar_win = syscall(SYS_CREATE_WINDOW, 0, 768 - 32, 1024, 32, (int)"Taskbar");
    
    // In a real system, the shell would check if it's in a "live" environment.
    // If so, it would display an "Install OS" icon that launches the installer.
    if (1) { // Assume we are in live mode
        if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
            char* path = "installer.elf";
            char* argv[] = {path, NULL};
            syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
        }
    }

    // Launch File Manager at startup
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "file_manager.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }
    
    // Launch Terminal at startup
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "terminal.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }

    // Launch Security Center
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "security_center.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }

    // Launch Linux Test App
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "linux_test.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }

    // Launch guitest app
    if (syscall(SYS_FORK, 0,0,0,0,0) == 0) {
        char* path = "guitest.elf";
        char* argv[] = {path, NULL};
        syscall(SYS_EXECVE, (int)path, (int)argv, 0, 0, 0);
    }

    event_t event;
    rtc_time_t time;
    char time_str[9]; // HH:MM:SS

    while(1) {
        // Redraw taskbar background
        syscall(SYS_DRAW_RECT, taskbar_win, 0, 0, 1024, 32, COLOR_TASKBAR, 0);

        // Get and draw time
        syscall(SYS_GET_SYSTEM_TIME, (int)&time, 0, 0, 0, 0);
        
        // This is a placeholder for a proper draw_string_to_window syscall
        // A real shell would use a syscall to draw text to its window buffer.
        // syscall(SYS_DRAW_STRING, taskbar_win, "Time", 980, 10, COLOR_FOREGROUND);
        
        if (syscall(SYS_POLL_EVENT, (int)&event, 0, 0, 0, 0)) {
            // Handle events, e.g., to launch new apps from the taskbar
        }
        syscall(SYS_YIELD, 0,0,0,0,0);
    }
}
