/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>

#include "common/crypto/kerl/converter.h"
#include "common/crypto/kerl/kerl.h"
#include "common/defs.h"

#define RATE 832
#define CAPACITY (1600 - RATE)
#define HASH_BIT_LEN 384
#define HASH_BYTE_LEN (HASH_BIT_LEN / 8)
#define HASH_INT_LEN (HASH_BYTE_LEN / 4)
#define SUFFIX 0x01

void kerl_init(Kerl* const ctx) {
  uint8_t succ = Keccak_HashInitialize(&ctx->keccak, 832, 768, 384, 0x01) == SUCCESS;
  assert(succ);
  (void)succ;
}

void kerl_absorb(Kerl* const ctx, trit_t const* trits, size_t const length) {
  uint8_t bytes[HASH_BYTE_LEN];
  trit_t const* const end = &trits[length];

  assert(length % HASH_LENGTH_TRIT == 0);

  for (; trits < end;) {
    convert_trits_to_bytes(trits, bytes);
    Keccak_HashUpdate(&ctx->keccak, bytes, HASH_BIT_LEN);
    trits = &trits[HASH_LENGTH_TRIT];
  }
}

void kerl_squeeze(Kerl* const ctx, trit_t* trits, size_t const length) {
  size_t i;
  uint8_t bytes[HASH_BYTE_LEN], tmp[HASH_BYTE_LEN];
  uint32_t* ptr = (uint32_t*)bytes;
  trit_t const* const end = &trits[length];

  assert(length % HASH_LENGTH_TRIT == 0);

  for (; trits < end;) {
    Keccak_HashSqueeze(&ctx->keccak, bytes, HASH_BIT_LEN);

    memcpy(tmp, bytes, HASH_BYTE_LEN * sizeof(uint8_t));
    convert_bytes_to_trits(tmp, trits);

    for (i = 0; i < HASH_INT_LEN; i++) {
      ptr[i] = ptr[i] ^ 0xFFFFFFFF;
    }

    kerl_init(ctx);
    Keccak_HashUpdate(&ctx->keccak, bytes, HASH_BIT_LEN);
    trits = &trits[HASH_LENGTH_TRIT];
  }
}

void kerl_reset(Kerl* const ctx) { kerl_init(ctx); }

#undef SUFFIX
#undef HASH_BIT_LEN
#undef HASH_BYTE_LEN
#undef HASH_INT_LEN
#undef CAPACITY
#undef RATE
