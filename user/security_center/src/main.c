#include <stdint.h>
#define SYS_CREATE_WINDOW 4
#define SYS_POLL_EVENT 6
#define EVENT_SYSTEM_ALERT 7

// ... Event struct and syscall definitions ...

void _start() {
    int win_id = syscall(SYS_CREATE_WINDOW, 500, 100, 400, 200, (int)"Security Center");
    
    event_t event;
    while(1) {
        if (syscall(SYS_POLL_EVENT, (int)&event, 0,0,0,0)) {
            if (event.type == EVENT_SYSTEM_ALERT) {
                // A real app would draw the alert message in the window
                // syscall(SYS_DRAW_STRING_IN_WINDOW, win_id, ...);
            }
        }
    }
}
