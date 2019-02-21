/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include "common/curl-p/trit.h"
#include "utils/forced_inline.h"

static FORCED_INLINE void sbox(trit_t *const lhs, trit_t *const rhs) {
  size_t i = 0;
  for (; i < STATE_LENGTH; ++i) {
    lhs[i] = TRUTH_TABLE[rhs[CURL_INDEX[i]] +
                         ((unsigned)rhs[CURL_INDEX[i + 1]] << 2) + 5];
  }
}

void curl_transform(trit_t *const state, size_t const rounds) {
  trit_t buffer[STATE_LENGTH];
  size_t i = 0;
  trit_t *lhs, *rhs;

  for (; i < rounds; ++i) {
    if (i & 1) {
      lhs = state;
      rhs = buffer;
    } else {
      lhs = buffer;
      rhs = state;
    }
    sbox(lhs, rhs);
  }
  if (i & 1) {
    memcpy(state, buffer, STATE_LENGTH);
  }
}

void init_curl(Curl *const ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }

void curl_absorb(Curl *const ctx, trit_t const *const trits, size_t length) {
  size_t num_chunks =
      length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(ctx->state, trits + i * HASH_LENGTH_TRIT,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
               sizeof(trit_t));
    curl_transform(ctx->state, (size_t)ctx->type);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void curl_squeeze(Curl *const ctx, trit_t *const trits, size_t length) {
  size_t num_chunks =
      length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(trits + i * HASH_LENGTH_TRIT, ctx->state,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
               sizeof(trit_t));
    curl_transform(ctx->state, (size_t)ctx->type);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void curl_reset(Curl *const ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }
