#ifndef PTY_H
#define PTY_H

#include "../fs/vfs.h"

// Terminal I/O Control commands
#define TIOCGWINSZ 0x5413 // Get window size

// Structure for window size
typedef struct {
    unsigned short ws_row;
    unsigned short ws_col;
    unsigned short ws_xpixel;
    unsigned short ws_ypixel;
} winsize_t;

void pty_init();
fs_node_t* pty_master_open(uint32_t flags);

#endif
