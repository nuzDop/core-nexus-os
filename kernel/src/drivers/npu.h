#ifndef NPU_H
#define NPU_H

#include <stdint.h>
#include <stdbool.h>

// Defines the types of tasks the NPU can perform
typedef enum {
    NPU_TASK_TYPE_INFER,
    NPU_TASK_TYPE_LEARN,
    NPU_TASK_TYPE_PREDICT,
} npu_task_type_t;

// Represents a single task to be processed by the NPU
typedef struct {
    npu_task_type_t type;
    uint32_t input_addr;
    uint32_t output_addr;
    uint32_t size;
} npu_task_t;

// Initializes the NPU driver.
void npu_init();

// Submits a task to the NPU for processing. Returns true on success.
bool npu_submit_task(npu_task_t* task);

#endif
