/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <stdlib.h>
#include <string.h>

#include "indices.h"
#include "ptrit.h"

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

void ptrit_transform_round(PCurl *const, PCurl *const, size_t const);
void ptrit_sbox(ptrit_t *const, ptrit_t const *const, size_t const);

void init_ptrit_curl(PCurl *const ctx) { ptrit_curl_reset(ctx); }

void ptrit_curl_absorb(PCurl *const ctx, ptrit_t const *const trits,
                       size_t const length) {
  memcpy(ctx->state, trits,
         (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
             sizeof(ptrit_t));
  ptrit_transform(ctx);
  if (length <= HASH_LENGTH_TRIT) {
    return;
  }
  ptrit_curl_absorb(ctx, trits + HASH_LENGTH_TRIT * sizeof(ptrit_t),
                    length - HASH_LENGTH_TRIT);
}

void ptrit_curl_squeeze(PCurl *const ctx, ptrit_t *const trits,
                        size_t const length) {
  memcpy(trits, ctx->state,
         (length < HASH_LENGTH_TRIT ? length : HASH_LENGTH_TRIT) *
             sizeof(ptrit_t));

  ptrit_transform(ctx);
  if (length <= HASH_LENGTH_TRIT) {
    return;
  }
  ptrit_curl_squeeze(ctx, trits + HASH_LENGTH_TRIT * sizeof(ptrit_t),
                     length - HASH_LENGTH_TRIT);
}

void ptrit_transform(PCurl *const ctx) {
  PCurl s;
  ptrit_transform_round(ctx, &s, ctx->type);
  memcpy(ctx->state, s.state, sizeof(ptrit_t) * STATE_LENGTH);
  ptrit_curl_reset(&s);
}

void ptrit_transform_round(PCurl *const ctx, PCurl *const s, size_t const i) {
  if (i == 0) {
    return;
  }
  ptrit_sbox(s->state, ctx->state, 0);
  ptrit_transform_round(s, ctx, i - 1);
}
void ptrit_sbox(ptrit_t *const c, ptrit_t const *const s, size_t const i) {
  if (i == STATE_LENGTH) {
    return;
  }
  ptrit_s alpha, beta, gamma, delta;
  alpha = s[CURL_INDEX[i]].low;
  beta = s[CURL_INDEX[i]].high;
  gamma = s[CURL_INDEX[i + 1]].high;
  delta = (alpha | (~gamma)) & (s[CURL_INDEX[i + 1]].low ^ beta);

  c->low = ~delta;
  c->high = (alpha ^ gamma) | delta;

  ptrit_sbox(&c[1], s, i + 1);
}

void ptrit_curl_reset(PCurl *const ctx) {
  memset(ctx->state, 0, sizeof(ptrit_t) * STATE_LENGTH);
}
