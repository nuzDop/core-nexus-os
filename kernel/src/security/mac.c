#include "mac.h"

// This is a simple, hardcoded security policy. A real system would load this from a file.
bool mac_check(security_context_t source_ctx, security_context_t target_ctx, mac_action_t action) {
    if (source_ctx == SEC_CTX_KERNEL) return true; // Kernel can do anything

    switch(action) {
        case MAC_ACTION_FILE_WRITE:
            // Rule: User applications cannot write to system files.
            if (source_ctx == SEC_CTX_USER && target_ctx == SEC_CTX_SYSTEM) {
                return false;
            }
            break;
        case MAC_ACTION_NET_BIND:
            // Rule: Sandboxed applications cannot bind to network ports.
            if (source_ctx == SEC_CTX_SANDBOXED) {
                return false;
            }
            break;
        case MAC_ACTION_EXEC:
            // Default allow, could add rules here.
            break;
    }

    return true; // Default policy: allow
}
