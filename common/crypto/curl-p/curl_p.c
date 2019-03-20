/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include "common/crypto/curl-p/trit.h"
#include "utils/forced_inline.h"
#include "utils/memset_safe.h"

static FORCED_INLINE void sbox(Curl *const c, Curl *const s) {
  size_t i = 0;
  for (; i < STATE_LENGTH; ++i) {
    c->state[i] = TRUTH_TABLE[s->state[CURL_INDEX[i]] + ((unsigned)s->state[CURL_INDEX[i + 1]] << 2) + 5];
  }
}

static void transform(Curl *const ctx) {
  Curl s;
  size_t round = 0;
  Curl *lhs, *rhs;

  for (; round < ctx->type; ++round) {
    if (round & 1) {
      lhs = ctx;
      rhs = &s;
    } else {
      lhs = &s;
      rhs = ctx;
    }
    sbox(lhs, rhs);
  }
  if (round & 1) memcpy(ctx->state, s.state, sizeof(ctx->state));
}

void curl_init(Curl *const ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }

void curl_absorb(Curl *const ctx, trit_t const *const trits, size_t length) {
  size_t num_chunks = length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(ctx->state, trits + i * HASH_LENGTH_TRIT,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) * sizeof(trit_t));
    transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void curl_squeeze(Curl *const ctx, trit_t *const trits, size_t length) {
  size_t num_chunks = length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;

  for (; i < num_chunks; ++i) {
    memcpy(trits + i * HASH_LENGTH_TRIT, ctx->state,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) * sizeof(trit_t));
    transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void curl_reset(Curl *const ctx) { memset_safe(ctx->state, sizeof(ctx->state), 0, sizeof(ctx->state)); }
