#include "window.h"
#include "../drivers/vbe.h"
#include "../mem/pmm.h"
#include "../gui/font.h"
#include "../proc/task.h"

static uint32_t* back_buffer;
static int screen_w = 1024;
static int screen_h = 768;

#define MAX_WINDOWS 10
static window_t* window_list[MAX_WINDOWS];
static int window_count = 0;
static int next_win_id = 1;

// State for window dragging
static window_t* dragged_window = NULL;
static int drag_offset_x = 0;
static int drag_offset_y = 0;

extern int mouse_x;
extern int mouse_y;

void init_window_manager() {
    back_buffer = (uint32_t*)pmm_alloc_page();
    for (int i = 0; i < MAX_WINDOWS; i++) window_list[i] = 0;
}

void fill_rect_buffer(int x, int y, int w, int h, uint32_t color, uint32_t* buffer, int buffer_w);
void draw_cursor_buffer(int x, int y);

void draw_window(window_t* win) {
    if (!win) return;
    
    // Copy the window's personal framebuffer to the back buffer
    for (int i = 0; i < win->height; i++) {
        for (int j = 0; j < win->width; j++) {
            int screen_x = win->x + j;
            int screen_y = win->y + i;
            if (screen_x >= 0 && screen_x < screen_w && screen_y >= 0 && screen_y < screen_h) {
                back_buffer[screen_y * screen_w + screen_x] = win->framebuffer[i * win->width + j];
            }
        }
    }
    
    // Draw the title bar on top of the screen's back_buffer, not the window's buffer
    fill_rect_buffer(win->x, win->y, win->width, 24, win->title_bar_color, back_buffer, screen_w);
    draw_string(win->title, win->x + 5, win->y + 8, 0xFFFFFF);
}

void compositor_redraw() {
    fill_rect_buffer(0, 0, screen_w, screen_h, 0x101520, back_buffer, screen_w);
    for (int i = 0; i < window_count; i++) {
        draw_window(window_list[i]);
    }
    // Draw cursor directly on the back_buffer
    fill_rect_buffer(mouse_x, mouse_y, 10, 10, 0xFFFFFF, back_buffer, screen_w);
    
    uint32_t* framebuffer = (uint32_t*)0xE0000000;
    for (int i = 0; i < screen_w * screen_h; i++) framebuffer[i] = back_buffer[i];
}

void fill_rect_buffer(int x, int y, int w, int h, uint32_t color, uint32_t* buffer, int buffer_w) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            if (j >= 0 && j < buffer_w && i >= 0 && i < screen_h) {
                // This logic is simplified; needs bounds checking for height as well
                buffer[i * buffer_w + j] = color;
            }
        }
    }
}

window_t* create_window(int x, int y, int w, int h, char* title) {
    if (window_count >= MAX_WINDOWS) return NULL;

    window_t* new_win = (window_t*)pmm_alloc_page();
    // A real implementation would request enough pages for the framebuffer
    new_win->framebuffer = (uint32_t*)pmm_alloc_page(); 
    new_win->id = next_win_id++;
    new_win->x = x;
    new_win->y = y;
    new_win->width = w;
    new_win->height = h;
    new_win->bg_color = 0x333333;
    new_win->title_bar_color = 0x00A8FF;
    new_win->title = title;
    new_win->is_dragging = false;
    new_win->owner_pid = current_task->id;

    fill_rect_buffer(0, 0, w, h, new_win->bg_color, new_win->framebuffer, w);

    window_list[window_count++] = new_win;
    return new_win;
}

// Helper to find a task by its PID
task_t* find_task_by_id(int id) {
    task_t* task = (task_t*)ready_queue;
    while(task) {
        if (task->id == id) return task;
        task = task->next;
    }
    return NULL;
}

// Helper to push an event to a task's queue
void push_event_to_task(task_t* task, event_t event) {
    if (!task) return;
    uint32_t next_head = (task->event_queue.head + 1) % EVENT_QUEUE_SIZE;
    if (next_head == task->event_queue.tail) {
        return; // Queue is full
    }
    task->event_queue.events[task->event_queue.head] = event;
    task->event_queue.head = next_head;
}

void wm_handle_mouse(int x, int y, bool left_button_down) {
    static bool was_button_down = false;

    if (dragged_window) {
        if (left_button_down) {
            dragged_window->x = x - drag_offset_x;
            dragged_window->y = y - drag_offset_y;
        } else {
            dragged_window->is_dragging = false;
            dragged_window = NULL;
        }
    } else {
        if (left_button_down && !was_button_down) {
            bool drag_started = false;
            for (int i = window_count - 1; i >= 0; i--) {
                window_t* win = window_list[i];
                if (x >= win->x && x < win->x + win->width && y >= win->y && y < win->y + 24) {
                    dragged_window = win;
                    win->is_dragging = true;
                    drag_offset_x = x - win->x;
                    drag_offset_y = y - win->y;
                    drag_started = true;
                    break;
                }
            }
            if (!drag_started) {
                 for (int i = window_count - 1; i >= 0; i--) {
                    window_t* win = window_list[i];
                    if (x >= win->x && x < win->x + win->width && y >= win->y && y < win->y + win->height) {
                        task_t* owner = find_task_by_id(win->owner_pid);
                        event_t event = { .type = EVENT_MOUSE_CLICK, .data1 = x - win->x, .data2 = y - win->y };
                        push_event_to_task(owner, event);
                        break;
                    }
                }
            }
        }
    }
    was_button_down = left_button_down;
}

void wm_draw_rect_in_window(int win_id, int x, int y, int w, int h, uint32_t color) {
    for (int i = 0; i < window_count; i++) {
        if (window_list[i]->id == win_id) {
            fill_rect_buffer(x, y, w, h, color, window_list[i]->framebuffer, window_list[i]->width);
            break;
        }
    }
}
