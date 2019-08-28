/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <string.h>

#include "common/trinary/ptrit.h"

#if defined(_MSC_VER)
#include <immintrin.h>
#if defined(_M_X64)
#define CTZ64(x) _tzcnt_u64(x)
#else
#define CTZ32(x) _tzcnt_u32(x)
#endif
#else
#define CTZ64(x) __builtin_ctzll(x)
#endif

// Not-a-Trit
#define NaT 2

static uint64_t const cvt_te1_to_tep[4][2] = {
#if defined(PTRIT_CVT_ANDN)
    // -1 -> (1,0); 0 -> (1,1); +1 -> (0,1)
    {1, 1},  // 0
    {0, 1},  // +1
    {0, 0},  // NaT
    {1, 0},  // -1
#elif defined(PTRIT_CVT_ORN)
    // -1 -> (0,0); 0 -> (0,1); +1 -> (1,1)
    {0, 1},  // 0
    {1, 1},  // +1
    {1, 0},  // NaT
    {0, 0},  // -1
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
};

static trit_t const cvt_tep_to_te1[2][2] = {
#if defined(PTRIT_CVT_ANDN)
    // -1 -> (1,0); 0 -> (1,1); +1 -> (0,1)
    {NaT, +1},
    {-1, 0},
#elif defined(PTRIT_CVT_ORN)
    // -1 -> (0,0); 0 -> (0,1); +1 -> (1,1)
    {-1, 0},
    {NaT, +1},
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
};

void ptrit_fill(ptrit_t *p, trit_t t) {
  // `t & 3` in order to avoid buffer overflow
  uint64_t const *c = cvt_te1_to_tep[t & 3];
  uint64_t p_low = (uint64_t)0 - c[0];
  uint64_t p_high = (uint64_t)0 - c[1];
  size_t i;

  for (i = 0; i < PTRIT_SIZE / 64; ++i) {
    ((uint64_t *)(&p->low))[i] = p_low;
    ((uint64_t *)(&p->high))[i] = p_high;
  }
}

void ptrit_set(ptrit_t *p, size_t idx, trit_t t) {
  // `t & 3` in order to avoid buffer overflow
  uint64_t const *c = cvt_te1_to_tep[t & 3];
  size_t w;

  assert(idx < PTRIT_SIZE);
  w = idx / 64;
  idx %= 64;
  ((uint64_t *)(&p->low))[w] &= ~(1ull << idx);
  ((uint64_t *)(&p->low))[w] |= c[0] << idx;
  ((uint64_t *)(&p->high))[w] &= ~(1ull << idx);
  ((uint64_t *)(&p->high))[w] |= c[1] << idx;
}

trit_t ptrit_get(ptrit_t const *p, size_t idx) {
  size_t w;
  size_t low, high;

  assert(idx < PTRIT_SIZE);
  w = idx / 64;
  idx %= 64;

  low = 1 & ((((uint64_t const *)(&p->low))[w]) >> idx);
  high = 1 & ((((uint64_t const *)(&p->high))[w]) >> idx);

  return cvt_tep_to_te1[low][high];
}

void ptrits_fill(size_t n, ptrit_t *dst, trit_t const *src) {
  for (; n--;) {
    ptrit_fill(dst++, *src++);
  }
}

void ptrits_set_slice(size_t n, ptrit_t *dst, size_t idx, trit_t const *src) {
  for (; n--;) {
    ptrit_set(dst++, idx, *src++);
  }
}

void ptrits_get_slice(size_t n, trit_t *dst, ptrit_t const *src, size_t idx) {
  for (; n--;) {
    *dst++ = ptrit_get(src++, idx);
  }
}

// Return index of all-zero n-trit slice or PTRIT_SIZE.
// TODO: return SIZE_MAX?
size_t ptrits_find_zero_slice(size_t n, ptrit_t const *p) {
  ptrit_s t;
  size_t i, w;

#if defined(PTRIT_CVT_ANDN)
  memset(&t, -1, sizeof(t));
  for (; n--; ++p) {
    // 0 -> (1,1)
    // t & (p->low & p->high)
    // zero bit indicates non-zero trit in that position
    t = ANDAND(t, p->low, p->high);
  }
#elif defined(PTRIT_CVT_ORN)
  memset(&t, 0, sizeof(t));
  t = XOR(t, t);
  for (; n--; ++p) {
    // 0 -> (0,1)
    // t | (~p->high | p->low)
    // non-zero bit indicates non-zero trit in that position
    t = ORORN(t, p->high, p->low);
  }
  t = NOT(t);
#else
#error Invalid PTRIT_CVT
#endif  // PTRIT_CVT

  // find right-most non-zero bit
#if defined(CTZ64)
  for (i = w = 0; w < PTRIT_SIZE / 64; ++w) {
    if (0 == ((uint64_t const *)(&t))[w]) {
      i += 64;
    } else {
      i += CTZ64(((uint64_t const *)(&t))[w]);
      break;
    }
  }
#elif defined(CTZ32)
  for (i = w = 0; w < PTRIT_SIZE / 32; ++w) {
    if (0 == ((uint32_t const *)(&t))[w]) {
      i += 32;
    } else {
      i += CTZ32(((uint32_t const *)(&t))[w]);
      break;
    }
  }
#else
#error CTZ not defined
#endif

  return i;
}

ssize_t ptrits_sum_slice(size_t n, ptrit_t const *p, size_t idx) {
  ssize_t sum = 0;
  for (; n--;) {
    sum += ptrit_get(p++, idx);
  }
  return sum;
}
