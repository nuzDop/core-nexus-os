#include <stdint.h>

typedef enum {
    EVENT_NONE, EVENT_MOUSE_PRESS, EVENT_MOUSE_RELEASE, EVENT_MOUSE_MOVE,
    EVENT_KEY_PRESS, EVENT_WINDOW_CLOSE, EVENT_BUTTON_CLICK
} event_type_t;

typedef struct {
    event_type_t type;
    int32_t data1;
    int32_t data2;
    int32_t data3;
} event_t;

#define SYS_CREATE_WINDOW   4
#define SYS_CREATE_WIDGET   25
#define SYS_POLL_EVENT      6
#define SYS_PRINT           1
#define WIDGET_BUTTON 0

int syscall(int num, int p1, int p2, int p3, int p4, int p5);

void _start() {
    int win_id = syscall(SYS_CREATE_WINDOW, 150, 150, 300, 200, (int)"GUI Test");
    int button_id = -1;

    if (win_id > 0) {
        button_id = syscall(SYS_CREATE_WIDGET, win_id, WIDGET_BUTTON, 110, 80, 80, 25, (int)"Click Me");
    }

    event_t event;
    while(1) {
        if (syscall(SYS_POLL_EVENT, (int)&event, 0, 0, 0, 0)) {
            if (event.type == EVENT_BUTTON_CLICK && event.data1 == button_id) {
                syscall(SYS_PRINT, (int)"Button was clicked!\n", 0, 0, 0, 0);
            }
        }
    }
}
