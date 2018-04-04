#include <assert.h>

#include "converter.h"
#include "kerl.h"

#define RATE 832
#define CAPACITY 1600 - RATE
#define HASH_BIT_LEN 384
#define HASH_BYTE_LEN HASH_BIT_LEN / 8
#define HASH_INT_LEN HASH_BYTE_LEN / 4
#define SUFFIX 0x01

void init_kerl(Kerl* ctx) {
  assert(Keccak_HashInitialize(&ctx->keccak, 832, 768, 384, 0x01) == SUCCESS);
}

void kerl_absorb(Kerl* ctx, trit_t* trits, size_t length) {
  uint8_t bytes[HASH_BYTE_LEN];
  trit_t* end = trits + length;

  assert(length % HASH_LENGTH == 0);

  for (; trits != end;) {
    trits_to_bytes(trits, bytes);
    Keccak_HashUpdate(&ctx->keccak, bytes, HASH_BIT_LEN);
    trits += HASH_LENGTH;
  }
}

void kerl_squeeze(Kerl* ctx, trit_t* trits, size_t length) {
  size_t i;
  uint8_t bytes[HASH_BYTE_LEN], tmp[HASH_BYTE_LEN];
  trit_t* end = trits + length;
  uint32_t* ptr = (uint32_t*)bytes;

  assert(length % HASH_LENGTH == 0);

  for (; trits != end;) {
    Keccak_HashSqueeze(&ctx->keccak, bytes, HASH_BIT_LEN);

    memcpy(tmp, bytes, HASH_BYTE_LEN * sizeof(uint8_t));
    bytes_to_trits(tmp, trits);

    for (i = 0; i < HASH_INT_LEN; i++) {
      ptr[i] = ptr[i] ^ 0xFFFFFFFF;
    }

    init_kerl(ctx);
    Keccak_HashUpdate(&ctx->keccak, bytes, HASH_BIT_LEN);
    trits += HASH_LENGTH;
  }
}

void kerl_reset(Kerl* ctx) { init_kerl(ctx); }

#undef SUFFIX
#undef HASH_BIT_LEN
#undef HASH_BYTE_LEN
#undef HASH_INT_LEN
#undef CAPACITY
#undef RATE
