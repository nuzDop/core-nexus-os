#include "nexus_core.h"
#include "../lib/string.h"
#include "../interrupts/syscall.h" // Include for syscall numbers

// Forward declare print for now. A proper kernel would have a dedicated debug header.
void print(const char* str);

// A very simple state model for a process
typedef struct {
    int pid;
    bool has_used_net;
    bool has_written_file;
} process_model_t;

#define MAX_MODELS 32
static process_model_t models[MAX_MODELS];
static int model_count = 0;

void nexus_core_init() {
    print("Nexus Core AI initialized.\n");
    model_count = 0;
}

void nexus_core_analyze_syscall(registers_t* regs) {
    if (!current_thread) return;

    // Find model for current PID
    process_model_t* model = NULL;
    for(int i = 0; i < model_count; i++) {
        if (models[i].pid == current_thread->parent_process->pid) {
            model = &models[i];
            break;
        }
    }
    if (!model && model_count < MAX_MODELS) {
        models[model_count].pid = current_thread->parent_process->pid;
        models[model_count].has_used_net = false;
        models[model_count].has_written_file = false;
        model = &models[model_count];
        model_count++;
    }

    if (!model) return;

    // Use rax for 64-bit syscall number
    if (model->has_used_net && regs->rax == SYS_WRITE) {
        if (!model->has_written_file) {
            print("NEXUS CORE ALERT: Process PID wrote to file after network activity.\n");
        }
    }
    
    // Update model state
    if (regs->rax >= SYS_SOCKET && regs->rax <= SYS_RECV) {
        model->has_used_net = true;
    }
    if (regs->rax == SYS_WRITE) {
        model->has_written_file = true;
    }
}
