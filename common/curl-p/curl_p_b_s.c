/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include "curl_p_b_s.h"

#include <stdio.h>
#include <string.h>

#define HASH_LENGTH S_STATE_LENGTH / 3
#define HALF_LENGTH S_STATE_LENGTH / 2
#define HALF_MAX HALF_LENGTH + 1
#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static const size_t CURL_INDEX[S_STATE_LENGTH + 1] = {__INDEX_TABLE};

static const trit_t TRUTH_TABLE[11] = {__TRUTH_TABLE};

void s_transform(bct_s_curl_t *ctx);
void transform_round(bct_s_curl_t *const, bct_s_curl_t *const, size_t);
void sbox(bct_s_curl_t *const, bct_s_curl_t *const, size_t const, size_t const);

void init_s_curl(bct_s_curl_t *ctx) {
  memset(ctx->state, 0, S_S_STATE_LENGTH * sizeof(bct_t));
}

void s_curl_absorb(bct_s_curl_t *ctx, bct_t *const trits, size_t offset,
                   size_t length) {
  copy_bct(ctx->state, 0, trits, offset,
           (length < HASH_LENGTH ? length : HASH_LENGTH));
  s_transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  s_curl_absorb(ctx, trits, offset + HASH_LENGTH, length - HASH_LENGTH);
}

void s_curl_squeeze(bct_s_curl_t *ctx, bct_t *const trits, size_t offset,
                    size_t length) {
  copy_bct(trits, offset, ctx->state, 0,
           (length < HASH_LENGTH ? length : HASH_LENGTH));
  s_transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  s_curl_squeeze(ctx, trits, offset + HASH_LENGTH, length - HASH_LENGTH);
}

void s_transform(bct_s_curl_t *const ctx) {
  bct_s_curl_t s;
  transform_round(ctx, &s, ctx->type == 27 ? 27 : 81);
}

void transform_round(bct_s_curl_t *const ctx, bct_s_curl_t *const s, size_t i) {
  if (i > 0) {
    memcpy(&(s->state[0]), &(ctx->state[0]), sizeof(ctx->state));
    sbox(ctx, s, 0, 0);
    transform_round(ctx, s, i - 1);
  }
}

void sbox(bct_s_curl_t *const c, bct_s_curl_t *const s, size_t const i,
          size_t const j) {
  if (i == S_STATE_LENGTH) {
    return;
  }
  trit_t a, b;
  size_t j_n = j <= HALF_LENGTH ? j + HALF_MAX : j - HALF_LENGTH;
  a = get_trit(s->state, j);
  b = get_trit(s->state, j_n);
  write_trit(c->state, i, TRUTH_TABLE[a + (b << 2) + 5]);

  sbox(c, s, i + 1, j_n);
}

void s_curl_reset(bct_s_curl_t *ctx) {
  memset(ctx->state, 0, S_S_STATE_LENGTH * sizeof(bct_t));
}
