#include "mac.h"
#include "../lib/string.h"
#include "../mem/pmm.h"

static security_context_t contexts[MAX_SEC_CONTEXTS];
static mac_rule_t rules[MAX_MAC_RULES];
static uint32_t context_count = 0;
static uint32_t rule_count = 0;

// This is a simplified parser. A real implementation would be more robust.
void mac_policy_load(const char* path) {
    fs_node_t* policy_file = finddir_fs(fs_root, (char*)path);
    if (!policy_file) {
        // PANIC("MAC: Security policy file not found!");
        return;
    }

    char* buffer = pmm_alloc_page();
    read_fs(policy_file, 0, PAGE_SIZE, (uint8_t*)buffer);

    // Parse the buffer line by line to populate the `contexts` and `rules` arrays.
    // Example line: "allow user_t system_bin_t file execute"
    // This logic would involve tokenizing the string and looking up
    // context names to get their IDs.
    
    context_count = 0; // Reset
    rule_count = 0;    // Reset

    // --- Hardcoded example policy for demonstration ---
    strcpy(contexts[0].name, "kernel_t"); contexts[0].id = 0;
    strcpy(contexts[1].name, "user_t");   contexts[1].id = 1;
    strcpy(contexts[2].name, "bin_t");    contexts[2].id = 2;
    context_count = 3;

    // Rule: allow user_t bin_t : file { execute };
    rules[0].source_type = 1; // user_t
    rules[0].target_type = 2; // bin_t
    rules[0].target_class = VFS_CLASS_FILE;
    rules[0].permissions = VFS_PERM_EXECUTE;
    rule_count = 1;
    // --- End of example policy ---

    pmm_free_page(buffer);
    print("MAC: Security policy loaded.\n");
}

bool mac_check(uint32_t source_sid, uint32_t target_sid, uint16_t target_class, uint32_t requested_perm) {
    if (source_sid == 0) return true; // Kernel is always allowed

    for (uint32_t i = 0; i < rule_count; i++) {
        if (rules[i].source_type == source_sid &&
            rules[i].target_type == target_sid &&
            rules[i].target_class == target_class)
        {
            // We found a matching rule. Check if the requested permission is allowed.
            if ((rules[i].permissions & requested_perm) == requested_perm) {
                return true; // Access granted
            }
        }
    }

    // No matching "allow" rule was found. Deny access.
    return false;
}

uint32_t mac_get_exec_transition(uint32_t old_sid, uint32_t file_sid) {
    // A full implementation would have specific "domain_transition" rules in the policy.
    // For now, we don't change the process context on exec.
    return old_sid;
}
