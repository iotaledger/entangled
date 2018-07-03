/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <string.h>

#include "curl_p_s.h"

#define S_HASH_LENGTH S_STATE_LENGTH / 3
#define HALF_LENGTH S_STATE_LENGTH / 2
#define HALF_MAX S_STATE_LENGTH / 2 + 1
#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static trit_t const TRUTH_TABLE[11] = {__TRUTH_TABLE};

void transform(s_curl_t *);
void transform_round(s_curl_t *const, s_curl_t *const, size_t const);
void sbox(s_curl_t *const, s_curl_t *const, size_t const, size_t const);

void s_init_curl(s_curl_t *const ctx) {
  memset(ctx->state, 0, sizeof(ctx->state));
}

void s_curl_absorb(s_curl_t *const ctx, trit_t const *const trits,
                   size_t const length) {
  memcpy(ctx->state, trits,
         (length < S_HASH_LENGTH ? length : S_HASH_LENGTH) * sizeof(trit_t));
  transform(ctx);
  if (length <= S_HASH_LENGTH) {
    return;
  }
  s_curl_absorb(ctx, trits + S_HASH_LENGTH * sizeof(trit_t),
                length - S_HASH_LENGTH);
}

void s_curl_squeeze(s_curl_t *const ctx, trit_t *const trits,
                    size_t const length) {
  memcpy(trits, ctx->state,
         (length < S_HASH_LENGTH ? length : S_HASH_LENGTH) * sizeof(trit_t));
  transform(ctx);
  if (length <= S_HASH_LENGTH) {
    return;
  }
  s_curl_squeeze(ctx, trits + S_HASH_LENGTH * sizeof(trit_t),
                 length - S_HASH_LENGTH);
}

void transform(s_curl_t *const ctx) {
  s_curl_t s;
  s_curl_reset(&s);
  transform_round(ctx, &s, ctx->type);
}

void transform_round(s_curl_t *const ctx, s_curl_t *const s, size_t const i) {
  if (i != 0) {
    memcpy(s->state, ctx->state, sizeof(ctx->state));
    sbox(ctx, s, 0, 0);
    transform_round(ctx, s, i - 1);
  }
}

void sbox(s_curl_t *const c, s_curl_t *const s, size_t const i,
          size_t const j) {
  if (i < S_STATE_LENGTH) {
    size_t j_n = j <= HALF_LENGTH ? j + HALF_LENGTH : j - HALF_MAX;
    c->state[i] = TRUTH_TABLE[s->state[j] + (s->state[j_n] << 2) + 5];
    sbox(c, s, i + 1, j_n);
  }
}

void s_curl_reset(s_curl_t *const ctx) {
  memset(ctx->state, 0, sizeof(ctx->state));
}
