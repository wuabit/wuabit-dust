#ifndef USHA256_H
#define USHA256_H

#include "stdint.h"

typedef struct {
  uint8_t datalen;
  union {
    uint32_t bitlen;
    uint8_t bitlen_bytes[4];
  };
  union {
    uint32_t state[8];
    uint8_t state_bytes[64];
  };
  union {
    uint8_t data[64];
    uint32_t data32[16];
  };
} Sha256Context;

// Initialize a SHA256 context
void sha256_init(Sha256Context *ctx);
// Update SHA256 hash with new message data
void sha256_update(Sha256Context *ctx, uint8_t *data, uint16_t len);
// Finalize and store SHA256 hash in hash
void sha256_final(Sha256Context *ctx, uint8_t hash[]);

#endif // USHA256_H
