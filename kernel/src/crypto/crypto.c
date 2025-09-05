#include "crypto.h"
#include "../lib/string.h"

// A cryptographically secure random number generator is required for a real implementation.
// For now, we use a placeholder. A real implementation would use hardware sources (RDRAND).
static uint64_t get_random_u64(void) {
    // Placeholder - NOT SECURE
    return 0xDEADBEEFCAFEBABE;
}

void crypto_init(void) {
    // Initialize the stack canary with a random value.
    extern uint64_t __stack_chk_guard;
    __stack_chk_guard = get_random_u64();
}

void sha256(const uint8_t *data, size_t len, uint8_t hash_out[SHA256_BLOCK_SIZE]) {
    // This is a placeholder for a full SHA-256 implementation.
    // A real implementation would involve a complex series of bitwise operations.
    // For now, we will just copy a portion of the input as a mock hash.
    for(size_t i = 0; i < SHA256_BLOCK_SIZE; ++i) {
        if (i < len) {
            hash_out[i] = data[i];
        } else {
            hash_out[i] = 0;
        }
    }
}
