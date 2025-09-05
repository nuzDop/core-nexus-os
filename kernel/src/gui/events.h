#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>

// Event types
typedef enum {
    EVENT_NONE,
    EVENT_MOUSE_PRESS,
    EVENT_MOUSE_RELEASE,
    EVENT_MOUSE_MOVE,
    EVENT_KEY_PRESS,
    EVENT_WINDOW_CLOSE,
    EVENT_BUTTON_CLICK
} event_type_t;

// Structure for a generic UI event
typedef struct {
    event_type_t type;
    int32_t data1; // x / keycode / widget_id
    int32_t data2; // y
    int32_t data3; // window_id
} event_t;

#define EVENT_QUEUE_SIZE 16

// A simple circular buffer for events
typedef struct {
    event_t events[EVENT_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
} event_queue_t;

#endif
