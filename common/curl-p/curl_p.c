/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <stdio.h>
#include <string.h>

#include "indices.h"
#include "trit.h"

#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

static const trit_t TRUTH_TABLE[11] = {__TRUTH_TABLE};

void transform(Curl *ctx);
void transform_round(Curl *const, Curl *const, size_t const);
void sbox(Curl *const, Curl *const, size_t const);

void init_curl(Curl *ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }

void curl_absorb(Curl *ctx, trit_t *const trits, size_t length) {
  memcpy(ctx->state, trits,
         (length < HASH_LENGTH ? length : HASH_LENGTH) * sizeof(trit_t));
  transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  curl_absorb(ctx, trits + HASH_LENGTH * sizeof(trit_t), length - HASH_LENGTH);
}

void curl_squeeze(Curl *ctx, trit_t *const trits, size_t length) {
  memcpy(trits, ctx->state,
         (length < HASH_LENGTH ? length : HASH_LENGTH) * sizeof(trit_t));
  transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  curl_squeeze(ctx, trits + HASH_LENGTH * sizeof(trit_t), length - HASH_LENGTH);
}

void transform(Curl *const ctx) {
  Curl s;
  transform_round(ctx, &s, ctx->type);
}

void transform_round(Curl *const ctx, Curl *const s, size_t const i) {
  if (i == 0) {
    return;
  }
  memcpy(s->state, ctx->state, sizeof(ctx->state));
  sbox(ctx, s, 0);
  transform_round(ctx, s, i - 1);
}

void sbox(Curl *const c, Curl *const s, size_t const i) {
  if (i == STATE_LENGTH) {
    return;
  }
  c->state[i] = TRUTH_TABLE[s->state[CURL_INDEX[i]] +
                            (s->state[CURL_INDEX[i + 1]] << 2) + 5];
  sbox(c, s, i + 1);
}

void curl_reset(Curl *ctx) { memset(ctx->state, 0, sizeof(ctx->state)); }
