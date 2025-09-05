#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <stdint.h>
#include <stdbool.h>
#include "window.h"
#include "widget.h"

void compositor_init(void);
void compositor_redraw(void);
void compositor_handle_mouse(int x, int y, bool left_press, bool left_release);
void compositor_add_window(window_t* win);
window_t* compositor_get_active_window(void);
void compositor_add_widget_to_window(window_t* win, widget_t* widget);

#endif
