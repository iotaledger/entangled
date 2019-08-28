/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>

#include "common/crypto/curl-p/ptrit.h"
#include "utils/forced_inline.h"
#include "utils/memset_safe.h"

static FORCED_INLINE void pcurl_s2(ptrit_t const *a, ptrit_t const *b, ptrit_t *c) {
#if defined(PTRIT_CVT_ORN)
  // (Xor AH (Orn BL AL),Xor AL (Orn BH (Xor AH (Orn BL AL))))
  c->low = XORORN(a->high, b->low, a->low);
  c->high = XORORN(a->low, b->high, c->low);
#elif defined(PTRIT_CVT_ANDN)
  // (Xor AH (Andn BL AL),Xor AL (And BH (Xor AH (Andn BL AL))))
  c->low = XORANDN(a->high, b->low, a->low);
  c->high = XORAND(a->low, b->high, c->low);
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
}

#if defined(PCURL_STATE_DOUBLE)
static FORCED_INLINE void pcurl_sbox(ptrit_t *c, ptrit_t const *s) {
  size_t i;

  // 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
  ptrit_t const *x = s + 0, *y = s + 364;
  pcurl_s2(x, y, c++);
  x = s + 728;

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 2; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#elif defined(PCURL_SBOX_UNWIND_8)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
    pcurl_s2(y--, x, c++);
    pcurl_s2(x--, y, c++);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif  // PCURL_SBOX_UNWIND
}

#elif defined(PCURL_STATE_SHORT)
// 0, 364, 728, 363, 727, ..., 2, 366, 1, 365, 0
// a : [  0..  364]-- => --[0,728..365]++ ->   xxxxxxxxxxxx   -> ++[0    ..364]
// b : [365..728,0]-- ->   xxxxxxxxxxxx   => --[364  ..  0]++ => ++[365..728,0]
// c : xxxxxxxxxxxx   => ++[0    ..364]-- => --[0,728..365]++ ->   xxxxxxxxxxxx
// c : xxxxxxxxxxxx   => --[364  ..  0]++ => ++[365..728,0]-- ->   xxxxxxxxxxxx
static FORCED_INLINE void pcurl_sbox_0(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  a = a + 364;
  b = b + 364;
  c = c + 0;
  ptrit_t *aa = a;
  ptrit_t *c0 = c;

  pcurl_s2(b--, a, c++);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
    pcurl_s2(a--, b, c++);
    pcurl_s2(b--, a, c++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
    pcurl_s2(a--, b, aa--);
    pcurl_s2(b--, a, aa--);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *aa = *c0;
}
static FORCED_INLINE void pcurl_sbox_1(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  c = c + 364;
  a = a + 0;
  b = b + 364;
  ptrit_t *cc = c;
  ptrit_t *b0 = b;

  pcurl_s2(a++, c, b--);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
    pcurl_s2(c--, a, b--);
    pcurl_s2(a++, c, b--);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
    pcurl_s2(c--, a, cc--);
    pcurl_s2(a++, c, cc--);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *cc = *b0;
}
static FORCED_INLINE void pcurl_sbox_2(ptrit_t *a, ptrit_t *b, ptrit_t *c) {
  size_t i;

  b = b + 0;
  c = c + 0;
  a = a + 0;
  ptrit_t *bb = b;
  ptrit_t *a0 = a;

  pcurl_s2(c++, b, a++);

#if defined(PCURL_SBOX_UNWIND_2)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 4; ++i) {
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
  }
#elif defined(PCURL_SBOX_UNWIND_4)
  // 728 = 8*91
  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
    pcurl_s2(b++, c, a++);
    pcurl_s2(c++, b, a++);
  }

  for (i = 0; i < CURL_STATE_SIZE / 8; ++i) {
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
    pcurl_s2(b++, c, bb++);
    pcurl_s2(c++, b, bb++);
  }
#else
#error Invalid PCURL_SBOX_UNWIND.
#endif

  *bb = *a0;
}
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE

void pcurl_init(pcurl_t *ctx, size_t round_count) {
  pcurl_reset(ctx);
  ctx->round_count = round_count;
}
void pcurl_absorb(pcurl_t *ctx, ptrit_t const *ptrits, size_t length) {
  size_t n = length / CURL_RATE;
  for (; n--;) {
    memcpy(ctx->state, ptrits, CURL_RATE * sizeof(ptrit_t));
    ptrits += CURL_RATE;
    // no padding!
    pcurl_transform(ctx);
  }
  length %= CURL_RATE;

  if (0 < length) {
    memcpy(ctx->state, ptrits, length * sizeof(ptrit_t));
    ptrits += length;
    // no padding!
    pcurl_transform(ctx);
    length = 0;
  }
}
void pcurl_squeeze(pcurl_t *ctx, ptrit_t *ptrits, size_t length) {
  size_t n = length / CURL_RATE;
  for (; n--;) {
    memcpy(ptrits, ctx->state, CURL_RATE * sizeof(ptrit_t));
    ptrits += CURL_RATE;
    // no padding!
    pcurl_transform(ctx);
  }
  length %= CURL_RATE;

  if (0 < length) {
    memcpy(ptrits, ctx->state, length * sizeof(ptrit_t));
    ptrits += length;
    // no padding!
    pcurl_transform(ctx);
    length = 0;
  }
}
void pcurl_get_hash(pcurl_t *ctx, ptrit_t *hash) {
  memcpy(hash, ctx->state, CURL_RATE * sizeof(ptrit_t));
  pcurl_reset(ctx);
}
void pcurl_hash_data(pcurl_t *ctx, ptrit_t const *data, size_t size, ptrit_t *hash) {
  pcurl_reset(ctx);
  pcurl_absorb(ctx, data, size);
  pcurl_get_hash(ctx, hash);
}

#if defined(PCURL_STATE_SHORT)
void pcurl_transform(pcurl_t *ctx) {
  size_t round;

  ptrit_t *a = ctx->state;
  ptrit_t *b = a + (CURL_STATE_SIZE + 1) / 2;
  ptrit_t *c = b + (CURL_STATE_SIZE + 1) / 2;
  b[364] = a[0];
  for (round = 0; round < ctx->round_count / 3; ++round) {
    pcurl_sbox_0(a, b, c);
    pcurl_sbox_1(a, b, c);
    pcurl_sbox_2(a, b, c);
  }
}
#elif defined(PCURL_STATE_DOUBLE)
void pcurl_transform(pcurl_t *ctx) {
  size_t round;

  ptrit_t *a = ctx->state, *t;
  ptrit_t *c = a + CURL_STATE_SIZE;
  for (round = 0; round < ctx->round_count; ++round) {
    pcurl_sbox(c, a);
    t = a;
    a = c;
    c = t;
  }
  if (1 & ctx->round_count) memcpy(c, a, sizeof(ptrit_t) * CURL_STATE_SIZE);
}
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE

void pcurl_reset(pcurl_t *ctx) {
#if defined(PTRIT_CVT_ANDN)
  // 0 -> (1,1)
  memset_safe(ctx->state, sizeof(ctx->state), -1, sizeof(ctx->state));

#elif defined(PTRIT_CVT_ORN)
  // 0 -> (0,1)
  size_t i;
#if defined(PCURL_STATE_SHORT)
  for (i = 0; i < 3 * (CURL_STATE_SIZE + 1) / 2; ++i)
#elif defined(PCURL_STATE_DOUBLE)
  for (i = 0; i < 2 * CURL_STATE_SIZE; ++i)
#else
#error Invalid PCURL_STATE.
#endif  // PCURL_STATE
  {
    memset_safe(&ctx->state[i].low, sizeof(ptrit_s), 0, sizeof(ptrit_s));
    memset_safe(&ctx->state[i].high, sizeof(ptrit_s), -1, sizeof(ptrit_s));
  }
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
}

void ptrit_curl_init(PCurl *const ctx, CurlType type) { pcurl_init(ctx, (size_t)type); }

void ptrit_curl_absorb(PCurl *const ctx, ptrit_t const *const trits, size_t length) {
  pcurl_absorb(ctx, trits, length);
}

void ptrit_curl_squeeze(PCurl *const ctx, ptrit_t *const trits, size_t length) { pcurl_squeeze(ctx, trits, length); }

void ptrit_transform(PCurl *const ctx) { pcurl_transform(ctx); }

void ptrit_curl_reset(PCurl *const ctx) { pcurl_reset(ctx); }
