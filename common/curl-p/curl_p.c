/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <stdio.h>
#include <string.h>

#include "common/curl-p/indices.h"
#include "common/curl-p/trit.h"

#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

static const trit_t TRUTH_TABLE[11] = {__TRUTH_TABLE};

void transform(Curl *const ctx);
void sbox(Curl *const, Curl *const);

void init_curl(Curl *const ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }

void curl_absorb(Curl *const ctx, trit_t const *const trits, size_t length) {
  size_t numChunks =
      length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;
  for (; i < numChunks; ++i) {
    memcpy(ctx->state, trits + i * HASH_LENGTH_TRIT * sizeof(trit_t),
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
               sizeof(trit_t));
    transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void curl_squeeze(Curl *const ctx, trit_t *const trits, size_t length) {
  size_t numChunks =
      length / HASH_LENGTH_TRIT + ((length % HASH_LENGTH_TRIT) ? 1 : 0);
  size_t i = 0;
  for (; i < numChunks; ++i) {
    memcpy(trits + i * HASH_LENGTH_TRIT * sizeof(trit_t), ctx->state,
           (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
               sizeof(trit_t));
    transform(ctx);
    length = length < HASH_LENGTH_TRIT ? 0 : length - HASH_LENGTH_TRIT;
  }
}

void transform(Curl *const ctx) {
  Curl s;
  size_t round = 0;
  for (; round < ctx->type; ++round) {
    if (round & 1)
      sbox(ctx, &s);
    else
      sbox(&s, ctx);
  }
  if (round & 1) memcpy(ctx->state, s.state, sizeof(ctx->state));
}

void sbox(Curl *const c, Curl *const s) {
  size_t i = 0;
  for (; i < STATE_LENGTH; ++i) {
    c->state[i] = TRUTH_TABLE[s->state[CURL_INDEX[i]] +
                              ((unsigned)s->state[CURL_INDEX[i + 1]] << 2) + 5];
  }
}

void curl_reset(Curl *const ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }
