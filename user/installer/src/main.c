/* user/installer/src/main.c */

#include <stdint.h>
#include <stdbool.h>

// --- Syscall Definitions ---
#define SYS_CREATE_WINDOW   4
#define SYS_CREATE_WIDGET   25
#define SYS_POLL_EVENT      6
#define SYS_FORK            17
#define SYS_EXECVE          18
#define SYS_YIELD           0
#define SYS_DRAW_RECT       5
#define SYS_DRAW_STRING_IN_WINDOW 30
// Assume other necessary syscalls like open, read, write, mount exist

// --- Theme and Layout ---
#define COLOR_BG 0x1E293B
#define COLOR_FG 0xF9FAFB
#define COLOR_BUTTON 0x334155

#define WIDGET_BUTTON 0
#define WIDGET_LABEL 1

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

// --- Installer State ---
typedef enum {
    PAGE_WELCOME,
    PAGE_COPYING,
    PAGE_FINISHED
} installer_page_t;

static installer_page_t current_page = PAGE_WELCOME;
static int win_id = -1;
static int next_button_id = -1;
static int cancel_button_id = -1;

void draw_page() {
    // Clear window
    syscall(SYS_DRAW_RECT, win_id, 0, 0, 500, 350, COLOR_BG, 0);

    switch (current_page) {
        case PAGE_WELCOME:
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"Welcome to LimitlessOS", 150, 50, COLOR_FG);
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"This will install the OS on your hard drive.", 80, 100, COLOR_FG);
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"Click Next to continue.", 160, 150, COLOR_FG);
            break;
        case PAGE_COPYING:
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"Installing...", 210, 50, COLOR_FG);
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"Formatting disk and copying files.", 110, 100, COLOR_FG);
            // In a real app, a progress bar would be drawn here
            break;
        case PAGE_FINISHED:
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"Installation Complete!", 160, 50, COLOR_FG);
            syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)"You can now reboot your computer.", 110, 100, COLOR_FG);
            break;
    }
}

void do_installation() {
    // This function simulates the actual installation process.
    
    // 1. Fork and exec mkfs to format the primary hard drive.
    if (syscall(SYS_FORK, 0, 0, 0, 0, 0) == 0) {
        char* path = "mkfs.elf";
        // A real execve would pass "/dev/hda" as an argument
        syscall(SYS_EXECVE, (int)path, 0, 0, 0, 0);
    }
    // A real implementation needs to wait for the child process to finish (waitpid syscall).

    // 2. Mount the new filesystem to a temporary location (e.g., /mnt)
    // syscall(SYS_MOUNT, "/dev/hda1", "/mnt", "limitlessfs");

    // 3. Copy files from the live ramdisk ("/") to the new mount point ("/mnt")
    // This involves recursively reading directories and copying files.
    // E.g., copy_file("/", "/mnt/", "shell.elf");
    //       copy_file("/", "/mnt/", "terminal.elf");
    //       ... etc. ...
}

void _start() {
    win_id = syscall(SYS_CREATE_WINDOW, 100, 100, 500, 350, (int)"LimitlessOS Installer");

    // Create persistent buttons
    next_button_id = syscall(SYS_CREATE_WIDGET, win_id, WIDGET_BUTTON, 400, 310, 80, 25, (int)"Next");
    cancel_button_id = syscall(SYS_CREATE_WIDGET, win_id, WIDGET_BUTTON, 20, 310, 80, 25, (int)"Cancel");

    draw_page();

    event_t event;
    while(1) {
        if (syscall(SYS_POLL_EVENT, (int)&event, 0, 0, 0, 0)) {
            if (event.type == EVENT_BUTTON_CLICK) {
                if (event.data1 == cancel_button_id) {
                    // Exit the installer (an exit syscall would be used here)
                    break;
                }
                if (event.data1 == next_button_id) {
                    if (current_page == PAGE_WELCOME) {
                        current_page = PAGE_COPYING;
                        draw_page();
                        // Perform the installation in the background
                        do_installation();
                        current_page = PAGE_FINISHED;
                        draw_page();
                    } else if (current_page == PAGE_FINISHED) {
                        // Exit
                        break;
                    }
                }
            }
        }
        syscall(SYS_YIELD, 0,0,0,0,0);
    }

    // A close window syscall would be called before exiting.
    while(1);
}
