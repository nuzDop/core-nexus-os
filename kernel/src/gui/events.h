#ifndef EVENTS_H
#define EVENTS_H

#include <stdint.h>

// Event types
#define EVENT_NONE          0
#define EVENT_MOUSE_CLICK   1
#define EVENT_MOUSE_MOVE    2
#define EVENT_KEY_PRESS     3

// Structure for a generic UI event
typedef struct {
    uint32_t type;
    int32_t data1;
    int32_t data2;
    int32_t data3;
} event_t;

#define EVENT_QUEUE_SIZE 16

// A simple circular buffer for events
typedef struct {
    event_t events[EVENT_QUEUE_SIZE];
    uint32_t head;
    uint32_t tail;
} event_queue_t;

#endif
