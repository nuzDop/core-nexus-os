#include <stdint.h>

#define SYS_CREATE_WINDOW 4
#define SYS_POLL_EVENT 6
#define SYS_FORK 17
#define SYS_EXECVE 18
#define SYS_READDIR 27
#define SYS_DRAW_RECT 5
#define SYS_DRAW_STRING_IN_WINDOW 30
#define SYS_DRAW_ICON_IN_WINDOW 31

#define COLOR_BODY 0x1E293B
#define COLOR_TEXT 0xF9FAFB
#define ICON_TYPE_FILE 0
#define ICON_TYPE_FOLDER 1

struct dirent {
    char name[128];
    uint32_t ino;
};

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

void redraw(int win_id, const char* path) {
    syscall(SYS_DRAW_RECT, win_id, 0, 0, 400, 300, COLOR_BODY, 0);

    struct dirent de;
    int index = 0;
    int y_pos = 10;
    while(syscall(SYS_READDIR, (int)path, index, (int)&de, 0, 0)) {
        // A real check for file type would use a stat syscall
        int is_dir = 0; // Placeholder
        syscall(SYS_DRAW_ICON_IN_WINDOW, win_id, is_dir ? ICON_TYPE_FOLDER : ICON_TYPE_FILE, 10, y_pos, 0);
        syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, (int)de.name, 30, y_pos + 4, COLOR_TEXT);
        y_pos += 20;
        index++;
    }
}

void _start() {
    int win_id = syscall(SYS_CREATE_WINDOW, 50, 50, 400, 300, (int)"File Manager");
    char current_path[128] = "/";

    redraw(win_id, current_path);

    event_t event;
    while(1) {
        if (syscall(SYS_POLL_EVENT, (int)&event, 0,0,0,0)) {
            if (event.type == 1 /* MOUSE_PRESS */) {
                // Check for double-click on a file entry
                // If executable, fork and execve it.
                // If directory, update current_path and call redraw().
            }
        }
    }
}
