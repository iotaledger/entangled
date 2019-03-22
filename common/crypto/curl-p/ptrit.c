/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <stdlib.h>
#include <string.h>

#include "common/crypto/curl-p/ptrit.h"
#include "utils/forced_inline.h"

static FORCED_INLINE void ptrit_sbox(ptrit_t *const c, ptrit_t const *const s) {
  ptrit_s alpha, beta, delta;
  size_t i = 0;

  for (; i < STATE_LENGTH; ++i) {
    alpha = s[CURL_INDEX[i]].low;
    beta = s[CURL_INDEX[i]].high;
    delta = alpha & (s[CURL_INDEX[i + 1]].low ^ beta);

    c[i].low = ~delta;
    c[i].high = (alpha ^ s[CURL_INDEX[i + 1]].high) | delta;
  }
}

void ptrit_curl_init(PCurl *const ctx, CurlType type) {
  ptrit_curl_reset(ctx);
  ctx->type = type;
}

void ptrit_curl_absorb(PCurl *const ctx, ptrit_t const *const trits, size_t length) {
  size_t num_chunks = length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(ctx->state, trits + i * HASH_LENGTH_TRIT,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) * sizeof(ptrit_t));
    ptrit_transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void ptrit_curl_squeeze(PCurl *const ctx, ptrit_t *const trits, size_t length) {
  size_t num_chunks = length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(trits + i * HASH_LENGTH_TRIT, ctx->state,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) * sizeof(ptrit_t));
    ptrit_transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void ptrit_transform(PCurl *const ctx) {
  PCurl s;
  size_t round = 0;
  ptrit_t *lhs, *rhs;

  for (; round < ctx->type; ++round) {
    if (round & 1) {
      lhs = ctx->state;
      rhs = s.state;
    } else {
      lhs = s.state;
      rhs = ctx->state;
    }
    ptrit_sbox(lhs, rhs);
  }

  if (round & 1) memcpy(ctx->state, s.state, sizeof(ctx->state));
}

void ptrit_curl_reset(PCurl *const ctx) {
  memset_safe(ctx->state, sizeof(ptrit_t) * STATE_LENGTH, HIGH_BITS, sizeof(ptrit_t) * STATE_LENGTH);
}
