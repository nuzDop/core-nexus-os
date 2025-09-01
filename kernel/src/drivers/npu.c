#include "npu.h"

void print(char*); // Forward declare from main.c

// Simulate NPU hardware registers and queue
#define NPU_QUEUE_SIZE 8
static npu_task_t npu_task_queue[NPU_QUEUE_SIZE];
static int queue_head = 0;
static int queue_tail = 0;
static bool npu_active = false;

void npu_init() {
    // A real driver would probe for hardware here.
    // We will just clear our simulated queue.
    for(int i = 0; i < NPU_QUEUE_SIZE; i++) {
        npu_task_queue[i].type = 0;
    }
    print("NPU driver initialized.\n");
}

bool npu_submit_task(npu_task_t* task) {
    int next_head = (queue_head + 1) % NPU_QUEUE_SIZE;
    if (next_head == queue_tail) {
        return false; // Queue is full
    }
    
    npu_task_queue[queue_head] = *task;
    queue_head = next_head;

    // Simulate the NPU starting to process if it was idle
    if (!npu_active) {
        // In a real driver, this would trigger a hardware interrupt upon completion.
        npu_active = true;
    }

    return true;
}
