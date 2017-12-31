/*
 * (c) 2016 Paul Handy, based on code from come-from-beyond
 */

#include <stdio.h>
#include <string.h>

#include "bct.h"
#include "indices.h"

#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

static const trit_t TRUTH_TABLE[11] = {__TRUTH_TABLE};

void s_transform(BCurl *ctx);
void transform_round(BCurl *const, BCurl *const, size_t);
void sbox(BCurl *const, BCurl *const, size_t const);

void init_s_curl(BCurl *ctx) {
  memset(ctx->state, 0, S_STATE_LENGTH * sizeof(bct_t));
}

void s_curl_absorb(BCurl *ctx, bct_t *const trits, size_t offset,
                   size_t length) {
  copy_bct(ctx->state, 0, trits, offset,
           (length < HASH_LENGTH ? length : HASH_LENGTH));
  s_transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  s_curl_absorb(ctx, trits, offset + HASH_LENGTH, length - HASH_LENGTH);
}

void s_curl_squeeze(BCurl *ctx, bct_t *const trits, size_t offset,
                    size_t length) {
  copy_bct(trits, offset, ctx->state, 0,
           (length < HASH_LENGTH ? length : HASH_LENGTH));
  s_transform(ctx);
  if (length <= HASH_LENGTH) {
    return;
  }
  s_curl_squeeze(ctx, trits, offset + HASH_LENGTH, length - HASH_LENGTH);
}

void s_transform(BCurl *const ctx) {
  BCurl s;
  transform_round(ctx, &s, ctx->type == 27 ? 27 : 81);
}

void transform_round(BCurl *const ctx, BCurl *const s, size_t i) {
  if (i > 0) {
    memcpy(&(s->state[0]), &(ctx->state[0]), sizeof(ctx->state));
    sbox(ctx, s, 0);
    transform_round(ctx, s, i - 1);
  }
}

void sbox(BCurl *const c, BCurl *const s, size_t const i) {
  if (i == STATE_LENGTH) {
    return;
  }
  trit_t a, b;
  a = get_trit(s->state, CURL_INDEX[i]);
  b = get_trit(s->state, CURL_INDEX[i + 1]);
  write_trit(c->state, i, TRUTH_TABLE[a + (b << 2) + 5]);

  sbox(c, s, i + 1);
}

void s_curl_reset(BCurl *ctx) {
  memset(ctx->state, 0, S_STATE_LENGTH * sizeof(bct_t));
}
