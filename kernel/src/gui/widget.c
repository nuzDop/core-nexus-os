#include "widget.h"
#include "../mem/pmm.h"
#include "../lib/string.h"
#include "font.h"

static int next_widget_id = 1;

widget_t* create_widget(widget_type_t type, window_t* parent, int x, int y, int w, int h, const char* text) {
    widget_t* new_widget = (widget_t*)pmm_alloc_page();
    new_widget->id = next_widget_id++;
    new_widget->type = type;
    new_widget->parent = parent;
    new_widget->x = x;
    new_widget->y = y;
    new_widget->width = w;
    new_widget->height = h;
    strcpy(new_widget->text, text);
    new_widget->needs_redraw = true;
    new_widget->on_click = NULL;

    return new_widget;
}

void draw_widget(widget_t* widget) {
    if (!widget || !widget->parent) return;

    if (widget->type == WIDGET_BUTTON) {
        // Draw button with theme colors
        draw_rect(widget->x, widget->y, widget->width, widget->height, COLOR_BUTTON_BG, widget->parent->buffer, widget->parent->width);
        
        // Draw text centered
        int text_len = strlen(widget->text);
        int text_x = widget->x + (widget->width - text_len * 8) / 2;
        int text_y = widget->y + (widget->height - 8) / 2;
        draw_string(widget->text, text_x, text_y, COLOR_BUTTON_FG, widget->parent->buffer, widget->parent->width);
    }
    
    widget->needs_redraw = false;
}
