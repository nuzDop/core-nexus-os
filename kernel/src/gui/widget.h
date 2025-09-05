#ifndef WIDGET_H
#define WIDGET_H

#include <stdint.h>
#include <stdbool.h>
#include "window.h"

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL
} widget_type_t;

typedef struct widget {
    int id;
    widget_type_t type;
    window_t* parent;
    int x, y, width, height;
    char text[64];
    bool needs_redraw;
    void (*on_click)(struct widget*);
} widget_t;

widget_t* create_widget(widget_type_t type, window_t* parent, int x, int y, int w, int h, const char* text);
void draw_widget(widget_t* widget);

#endif
