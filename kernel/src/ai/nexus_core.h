#ifndef NEXUS_CORE_H
#define NEXUS_CORE_H

#include <stdint.h>
#include <stdbool.h>
#include "../proc/task.h"

void nexus_core_init();
void nexus_core_analyze_syscall(registers_t* regs);

#endif
