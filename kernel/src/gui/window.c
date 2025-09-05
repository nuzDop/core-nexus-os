#include "window.h"
#include "../mem/pmm.h"
#include "compositor.h"
#include "../proc/task.h"

static int next_win_id = 1;

window_t* create_window(int x, int y, int w, int h, char* title) {
    window_t* new_win = (window_t*)pmm_alloc_page();
    new_win->buffer = (uint32_t*)pmm_alloc_page(); 
    new_win->id = next_win_id++;
    new_win->x = x;
    new_win->y = y;
    new_win->width = w;
    new_win->height = h;
    new_win->title = title;
    new_win->owner_pid = current_task->id;
    new_win->widget_count = 0;

    compositor_add_window(new_win);
    return new_win;
}

void destroy_window(window_t* win) {
    if (!win) return;
    pmm_free_page(win->buffer);
    pmm_free_page(win);
}
