#ifndef NEXUS_CORE_H
#define NEXUS_CORE_H

#include <stdint.h>
#include "../proc/task.h"

// Represents a snapshot of a syscall for AI analysis
typedef struct {
    int syscall_number;
    int owner_pid;
    uint32_t param1;
    uint32_t param2;
    uint32_t param3;
    uint32_t param4;
} nexus_syscall_packet_t;

// Initializes The Nexus Core AI subsystem.
void nexus_core_init();

// The main entry point for the AI to analyze system activity.
// This is called by the syscall dispatcher for every system call.
void nexus_core_analyze_syscall(registers_t* regs);

#endif
