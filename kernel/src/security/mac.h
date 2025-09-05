#ifndef MAC_H
#define MAC_H

#include <stdbool.h>
#include "../proc/task.h"
#include "../fs/vfs.h"

// Mandatory Access Control
typedef enum {
    SEC_CTX_UNCONFINED,
    SEC_CTX_KERNEL,
    SEC_CTX_SYSTEM,     // System daemons and servers
    SEC_CTX_USER,       // User applications
    SEC_CTX_SANDBOXED   // Untrusted applications
} security_context_t;

// Action types for policy checks
typedef enum {
    MAC_ACTION_EXEC,
    MAC_ACTION_FILE_WRITE,
    MAC_ACTION_NET_BIND
} mac_action_t;

// Check if a source context is allowed to perform an action on a target context
bool mac_check(security_context_t source_ctx, security_context_t target_ctx, mac_action_t action);

#endif
