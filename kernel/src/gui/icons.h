#ifndef ICONS_H
#define ICONS_H

#include <stdint.h>

#define ICON_WIDTH 16
#define ICON_HEIGHT 16

extern const uint32_t icon_file[ICON_WIDTH * ICON_HEIGHT];
extern const uint32_t icon_folder[ICON_WIDTH * ICON_HEIGHT];

typedef enum {
    ICON_TYPE_FILE,
    ICON_TYPE_FOLDER
} icon_type_t;

#endif
