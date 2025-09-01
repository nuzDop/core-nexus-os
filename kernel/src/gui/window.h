#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    int id;
    int owner_pid; // The ID of the process that owns this window
    int x, y;
    int width, height;
    uint32_t bg_color;
    uint32_t title_bar_color;
    char* title;
    bool is_dragging;
    uint32_t* framebuffer;
} window_t;

void init_window_manager();
void compositor_redraw();
void draw_window(window_t* win);
window_t* create_window(int x, int y, int w, int h, char* title);
void wm_handle_mouse(int x, int y, bool left_button_down);
void wm_draw_rect_in_window(int win_id, int x, int y, int w, int h, uint32_t color);

#endif
