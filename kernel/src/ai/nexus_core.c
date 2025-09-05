#include "nexus_core.h"
#include "../proc/task.h"
#include "../lib/string.h"

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
    if (!current_task) return;

    // Find model for current PID
    process_model_t* model = NULL;
    for(int i = 0; i < model_count; i++) {
        if (models[i].pid == current_task->id) {
            model = &models[i];
            break;
        }
    }
    if (!model && model_count < MAX_MODELS) {
        models[model_count].pid = current_task->id;
        models[model_count].has_used_net = false;
        models[model_count].has_written_file = false;
        model = &models[model_count];
        model_count++;
    }

    if (!model) return;

    // Simple Anomaly Detection Rule: If a process that has never used the network
    // suddenly tries to write to a file, flag it as suspicious.
    if (model->has_used_net && regs->eax == SYS_WRITE) {
        if (!model->has_written_file) {
            print("NEXUS CORE ALERT: Process PID %d wrote to file after network activity. Potential exfiltration attempt.\n");
            // In a real system, this would send an EVENT_SYSTEM_ALERT
        }
    }
    
    // Update model state
    if (regs->eax >= SYS_SOCKET && regs->eax <= SYS_RECV) {
        model->has_used_net = true;
    }
    if (regs->eax == SYS_WRITE) {
        model->has_written_file = true;
    }
}
