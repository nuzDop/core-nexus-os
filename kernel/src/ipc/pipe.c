/* kernel/src/ipc/pipe.c */

#include "pipe.h"
#include "../mem/pmm.h"
#include "../proc/task.h"
#include "../lib/string.h"

#define PIPE_SIZE 4096 // 4KB buffer

// Represents the shared buffer and state for a pipe
typedef struct {
    uint8_t buffer[PIPE_SIZE];
    uint32_t write_pos; // Position of next write
    uint32_t read_pos;  // Position of next read
    uint32_t data_size; // Number of bytes currently in buffer
    // In a preemptive kernel, you'd need locks and wait queues here
} pipe_buffer_t;

// Structure that fs_nodes will point to
typedef struct {
    pipe_buffer_t* buffer;
    bool is_write_end; // Differentiates the read and write fs_nodes
} pipe_device_t;

// Called when a process reads from the read-end of the pipe
static uint32_t pipe_read(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)offset; // Pipes don't have offsets
    pipe_device_t* device = (pipe_device_t*)node->ptr;
    pipe_buffer_t* pipe_buf = device->buffer;

    // A real implementation would block here if pipe_buf->data_size is 0
    // while(pipe_buf->data_size == 0) { sleep(); }

    uint32_t read_count = 0;
    while (read_count < size && pipe_buf->data_size > 0) {
        buffer[read_count] = pipe_buf->buffer[pipe_buf->read_pos];
        pipe_buf->read_pos = (pipe_buf->read_pos + 1) % PIPE_SIZE;
        pipe_buf->data_size--;
        read_count++;
    }

    // A real implementation would wake up any waiting writers here.

    return read_count;
}

// Called when a process writes to the write-end of the pipe
static uint32_t pipe_write(fs_node_t* node, uint32_t offset, uint32_t size, uint8_t* buffer) {
    (void)offset;
    pipe_device_t* device = (pipe_device_t*)node->ptr;
    pipe_buffer_t* pipe_buf = device->buffer;
    
    // A real implementation would block here if pipe_buf->data_size is PIPE_SIZE
    // while(pipe_buf->data_size == PIPE_SIZE) { sleep(); }

    uint32_t write_count = 0;
    while (write_count < size && pipe_buf->data_size < PIPE_SIZE) {
        pipe_buf->buffer[pipe_buf->write_pos] = buffer[write_count];
        pipe_buf->write_pos = (pipe_buf->write_pos + 1) % PIPE_SIZE;
        pipe_buf->data_size++;
        write_count++;
    }

    // A real implementation would wake up any waiting readers here.

    return write_count;
}

// Finds the next available file descriptor for the current task
static int find_free_fd() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (current_task->file_descriptors[i] == NULL) {
            return i;
        }
    }
    return -1; // No free file descriptors
}

int create_pipe(int* fds) {
    pipe_buffer_t* buffer = (pipe_buffer_t*)pmm_alloc_page();
    memset(buffer, 0, sizeof(pipe_buffer_t));

    // Create the read and write ends
    fs_node_t* read_node = (fs_node_t*)pmm_alloc_page();
    memset(read_node, 0, sizeof(fs_node_t));
    strcpy(read_node->name, "pipe_read");
    read_node->flags = FS_PIPE;
    read_node->read = &pipe_read;

    fs_node_t* write_node = (fs_node_t*)pmm_alloc_page();
    memset(write_node, 0, sizeof(fs_node_t));
    strcpy(write_node->name, "pipe_write");
    write_node->flags = FS_PIPE;
    write_node->write = &pipe_write;
    
    // Create device wrappers
    pipe_device_t* read_dev = (pipe_device_t*)pmm_alloc_page();
    read_dev->buffer = buffer;
    read_dev->is_write_end = false;
    read_node->ptr = read_dev;

    pipe_device_t* write_dev = (pipe_device_t*)pmm_alloc_page();
    write_dev->buffer = buffer;
    write_dev->is_write_end = true;
    write_node->ptr = write_dev;

    // Assign file descriptors
    int read_fd = find_free_fd();
    if (read_fd == -1) return -1;
    current_task->file_descriptors[read_fd] = read_node;

    int write_fd = find_free_fd();
    if (write_fd == -1) return -1;
    current_task->file_descriptors[write_fd] = write_node;
    
    fds[0] = read_fd;
    fds[1] = write_fd;
    
    return 0; // Success
}
