#include "usha256.h"
#include <avr/pgmspace.h>
#include <string.h>

#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

const PROGMEM uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
    0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
    0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
    0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
    0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
    0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

const PROGMEM uint32_t h[8] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
                           0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

__attribute__((noinline)) uint32_t sha256_rotr(uint32_t a, uint8_t b) {
  return (a >> b) | (a << (32 - b));
}

uint32_t sha256_sig0(uint32_t x) {
  return sha256_rotr(x, 7) ^ sha256_rotr(x, 18) ^ (x >> 3);
}

uint32_t sha256_sig1(uint32_t x) {
  return sha256_rotr(x, 17) ^ sha256_rotr(x, 19) ^ (x >> 10);
}

uint32_t sha256_ep0(uint32_t x) {
  return sha256_rotr(x, 2) ^ sha256_rotr(x, 13) ^ sha256_rotr(x, 22);
}

uint32_t sha256_ep1(uint32_t x) {
  return sha256_rotr(x, 6) ^ sha256_rotr(x, 11) ^ sha256_rotr(x, 25);
}

void sha256_memcpy_swap_endianness(uint8_t *dst, uint8_t *src, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    *(dst++) = src[i^0x03];
  }
}

void sha256_transform(Sha256Context *ctx) {
  uint8_t i;
  uint32_t t1, t2;
  union {
    uint32_t state[8];
    uint8_t state_bytes[32];
  };
  memcpy(state, ctx->state, 32);
  for (i = 0; i < 64; ++i) {
    uint32_t m;
    if (i < 16) {
      m = ctx->data32[i];
    } else {
      m = sha256_sig1(ctx->data32[(i - 2)%16])
          + ctx->data32[(i - 7)%16]
          + sha256_sig0(ctx->data32[(i - 15)%16])
          + ctx->data32[(i - 16)%16];
      ctx->data32[i%16] = m;
    }
    t1 = state[7] + sha256_ep1(state[4]) + CH(state[4], state[5], state[6]) +
         pgm_read_dword(k + i) + m;
    t2 = sha256_ep0(state[0]) + MAJ(state[0], state[1], state[2]);
    // shift g to h, f to g, etc.
    for (uint8_t j = 31; j >= 4; j--) {
      state_bytes[j] = state_bytes[j - 4];
    }
    state[4] += t1;      // e
    state[0] = t1 + t2;  // a
  }
  for (i = 0; i < 8; i++) {
    ctx->state[i] += state[i];
  }
}

void sha256_init(Sha256Context *ctx) {
  memset(ctx, 0, 8);
  for (uint8_t i = 0; i < 32; i++) {
    ctx->state_bytes[i] = pgm_read_byte((uint8_t *)h + i);
  }
}

void sha256_update(Sha256Context *ctx, uint8_t *data, uint16_t len) {
  for (; len > 0; len--) {
    // swap endianness of incoming data
    ctx->data[ctx->datalen^0x03] = *(data++);
    ctx->datalen++;
    if (ctx->datalen == 64) {
      sha256_transform(ctx);
      ctx->bitlen += 512;
      ctx->datalen = 0;
    }
  }
}

void sha256_final(Sha256Context *ctx, uint8_t hash[]) {
  uint32_t i;
  // pad message if needed
  i = ctx->datalen;
  if (ctx->datalen < 56) {
    // enough room to append bitlength after padding
    ctx->data[(i++)^0x03] = 0x80;
    while (i < 60) {
      ctx->data[(i++)^0x03] = 0x00;
    }
  } else {
    // not enough room to append bitlength, transform this block and start
    // a new one
    ctx->data[(i++)^0x03] = 0x80;
    while (i < 64) {
      ctx->data[(i++)^0x03] = 0x00;
    }
    sha256_transform(ctx);
    memset(ctx->data, 0, 60);
  }
  ctx->bitlen += ctx->datalen * 8;
  ctx->data32[15] = ctx->bitlen;
  sha256_transform(ctx);
  sha256_memcpy_swap_endianness(hash, ctx->state_bytes, 32);
}
