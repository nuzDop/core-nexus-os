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
    if (!widget || !widget->needs_redraw) return;

    if (widget->type == WIDGET_BUTTON) {
        // Draw button background and border
        draw_rect(widget->x, widget->y, widget->width, widget->height, 0x555555, widget->parent->buffer, widget->parent->width);
        draw_rect(widget->x + 1, widget->y + 1, widget->width - 2, widget->height - 2, 0x777777, widget->parent->buffer, widget->parent->width);
        
        // Draw text centered
        int text_len = strlen(widget->text);
        int text_x = widget->x + (widget->width - text_len * 8) / 2;
        int text_y = widget->y + (widget->height - 8) / 2;
        draw_string(widget->text, text_x, text_y, 0xFFFFFF, widget->parent->buffer, widget->parent->width);
    }

    widget->needs_redraw = false;
}
