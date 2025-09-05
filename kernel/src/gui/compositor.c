#include "compositor.h"
#include "../drivers/vbe.h"
#include "../mem/pmm.h"
#include "font.h"
#include "theme.h"
#include "../lib/string.h"
#include "../proc/task.h"

static uint32_t* back_buffer;
static uint32_t screen_w, screen_h;

static window_t* active_window = NULL;

#define MAX_WINDOWS 16
static window_t* window_list[MAX_WINDOWS];
static int window_count = 0;
static int z_order[MAX_WINDOWS];

static window_t* dragged_window = NULL;
static int drag_offset_x = 0;
static int drag_offset_y = 0;

extern int mouse_x;
extern int mouse_y;

void compositor_handle_mouse(int x, int y, bool left_press, bool left_release) {
    // This function needs to be rewritten to handle widget clicks and window dragging.
}

void compositor_add_widget_to_window(window_t* win, widget_t* widget) {
    // In a real implementation, windows would have a list of widgets.
    // For now, we'll just draw the widget immediately for simplicity.
    draw_widget(widget);
}

void compositor_init(void) {
    screen_w = vbe_get_width();
    screen_h = vbe_get_height();
    // Allocate a back buffer for double buffering
    back_buffer = (uint32_t*)pmm_alloc_page(); // Simplified allocation
    for (int i = 0; i < MAX_WINDOWS; i++) {
        window_list[i] = 0;
        z_order[i] = -1;
    }
}

static void draw_rect(int x, int y, int w, int h, uint32_t color, uint32_t* buffer, int buffer_w) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            if (j >= 0 && j < buffer_w && i >= 0 && i < (int)screen_h) {
                buffer[i * buffer_w + j] = color;
            }
        }
    }
}

void compositor_redraw(void) {
    // 1. Draw desktop background
    draw_rect(0, 0, screen_w, screen_h, COLOR_BACKGROUND, back_buffer, screen_w);

    // 2. Draw windows from bottom to top
    for (int i = 0; i < window_count; i++) {
        int win_index = z_order[i];
        if (win_index != -1 && window_list[win_index]) {
            window_t* win = window_list[win_index];

            uint32_t border_color = win->is_active ? COLOR_WINDOW_BORDER_ACTIVE : COLOR_WINDOW_BORDER_INACTIVE;
            
            // Draw title bar (no separate border, just a solid bar)
            draw_rect(win->x, win->y - TITLEBAR_HEIGHT, win->width, TITLEBAR_HEIGHT, COLOR_WINDOW_TITLEBAR, back_buffer, screen_w);
            draw_string(win->title, win->x + 8, win->y - 18, COLOR_WINDOW_TITLE_TEXT, back_buffer, screen_w);
            
            // Draw window body background and copy contents
            draw_rect(win->x, win->y, win->width, win->height, COLOR_WINDOW_BODY, back_buffer, screen_w);
            for (int row = 0; row < win->height; row++) {
                memcpy(&back_buffer[(win->y + row) * screen_w + win->x], &win->buffer[row * win->width], win->width * 4);
            }
        }
    }

    // 3. Draw mouse cursor
    draw_rect(mouse_x, mouse_y, 1, 15, COLOR_FOREGROUND, back_buffer, screen_w);
    draw_rect(mouse_x, mouse_y, 8, 1, COLOR_FOREGROUND, back_buffer, screen_w);


    // 4. Swap buffers
    memcpy(vbe_get_framebuffer(), back_buffer, screen_w * screen_h * 4);
}
// Other functions from old window.c refactored for the compositor...
// (wm_handle_mouse, wm_draw_rect_in_window, create_window etc. go here)

void compositor_add_window(window_t* win) {
    if (window_count >= MAX_WINDOWS) return;
    window_list[window_count] = win;
    z_order[window_count] = window_count;
    active_window = win;
    window_count++;
}

window_t* compositor_get_active_window(void) {
    return active_window;
}

void compositor_handle_mouse(int x, int y, bool left_press, bool left_release) {
    if (left_press) {
        // Find the top-most window under the cursor
        for (int i = window_count - 1; i >= 0; i--) {
            int win_index = z_order[i];
            window_t* win = window_list[win_index];
            if (x >= win->x && x < win->x + win->width && y >= win->y && y < win->y + win->height) {
                active_window = win;

                // Check for widget clicks
                for (int j = 0; j < win->widget_count; j++) {
                    widget_t* widget = win->widgets[j];
                    if (x >= win->x + widget->x && x < win->x + widget->x + widget->width &&
                        y >= win->y + widget->y && y < win->y + widget->y + widget->height) {
                        
                        task_t* owner = find_task_by_id(win->owner_pid);
                        if(owner) {
                            event_t event = { .type = EVENT_BUTTON_CLICK, .data1 = widget->id, .data3 = win->id };
                            push_event_to_task(owner, event);
                        }
                        return; // Event handled
                    }
                }
                
                // If no widget was clicked, it's a general mouse press on the window
                task_t* owner = find_task_by_id(win->owner_pid);
                if(owner) {
                    event_t event = { .type = EVENT_MOUSE_PRESS, .data1 = x - win->x, .data2 = y - win->y, .data3 = win->id };
                    push_event_to_task(owner, event);
                }
                return;
            }
        }
    }
}
