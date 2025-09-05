#ifndef MAC_H
#define MAC_H

#include <stdbool.h>
#include "../proc/task.h"
#include "../fs/vfs.h"

#define MAX_SEC_CONTEXTS 64
#define MAX_MAC_RULES 256

// An in-memory representation of a security context type
typedef struct {
    uint32_t id;
    char name[64];
} security_context_t;

// An in-memory representation of a single Type Enforcement rule
// e.g., allow user_t system_file_t : file { read execute };
typedef struct {
    uint32_t source_type;  // e.g., ID for "user_t"
    uint32_t target_type;  // e.g., ID for "system_file_t"
    uint16_t target_class; // e.g., CLASS_FILE
    uint32_t permissions;  // Bitmask of allowed actions
} mac_rule_t;

// --- Public MAC Functions ---

// Loads the policy from a file on the root filesystem
void mac_policy_load(const char* path);

// Checks if a source context is allowed to perform an action on a target context
bool mac_check(uint32_t source_sid, uint32_t target_sid, uint16_t target_class, uint32_t requested_perm);

// Determines the security context for a new process during execve
uint32_t mac_get_exec_transition(uint32_t old_sid, uint32_t file_sid);

#endif
