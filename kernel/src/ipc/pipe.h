#ifndef PIPE_H
#define PIPE_H

#include <stdint.h>

// Creates a pipe and returns the file descriptor for the read end.
// The write end is returned in the `write_fd` parameter.
int create_pipe(int* write_fd);

#endif
