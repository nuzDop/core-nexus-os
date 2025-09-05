#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define SHA256_BLOCK_SIZE 32

// Function prototypes for cryptographic operations

/**
 * @brief Computes the SHA-256 hash of a message.
 * * @param data Pointer to the data to hash.
 * @param len Length of the data in bytes.
 * @param hash_out Buffer to store the 32-byte hash result.
 */
void sha256(const uint8_t *data, size_t len, uint8_t hash_out[SHA256_BLOCK_SIZE]);

/**
 * @brief Initializes the kernel cryptography subsystem.
 */
void crypto_init(void);

#endif
