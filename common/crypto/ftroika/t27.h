/*
 * Copyright (c) 2019 IOTA Stiftung
 * Copyright (c) 2019 Cybercrypt A/S
 * https://github.com/iotaledger/entangled
 * Based on c-mnd implementation: https://github.com/c-mnd/troika
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
 * "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
 * LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __COMMON_FTROIKA_T27_H__
#define __COMMON_FTROIKA_T27_H__

#include <inttypes.h>
#include <stdio.h>

#include "common/crypto/ftroika/general.h"
#include "common/trinary/trits.h"

#define T27_NUM_COLUMNS 9
#define T27_NUM_SLICES 27
#define T27_NUM_ROWS 3
#define T27_SLICE_SIZE (T27_NUM_COLUMNS * T27_NUM_ROWS)

// trits are stored as two bits, one inside p, the other inside n, both have the
// same 'index' names 'p' (positive) and 'n' (negative) result from using these
// functions in a lib for balanced ternary before so i just left them here

typedef struct {
  uint32_t p;
  uint32_t n;
} t27_t;

static inline t27_t t27_init(const uint32_t a, const uint32_t b) {
  t27_t r;
  r.p = a;
  r.n = b;
  return r;
}

static inline int t27_eq(const t27_t a, const t27_t b) {
  if (a.p == b.p && a.n == b.n) {
    return 1;
  }
  return 0;
}

static inline t27_t t27_clean(t27_t a) {
  t27_t r;
  r.p = a.p & 0x07ffffff;
  r.n = a.n & 0x07ffffff;
  return r;
}

static inline t27_t t27_sum(const t27_t* a, const t27_t* b) {
  t27_t r;
  r.p = (_1(a) & _0(b)) | (_0(a) & _1(b)) | (_2(a) & _2(b));
  r.n = (_2(a) & _0(b)) | (_0(a) & _2(b)) | (_1(a) & _1(b));
  return r;
}

static inline t27_t t27_roll(t27_t a, const int n) {
  t27_t r;
  r.p = ((a.p << n) | (a.p >> (27 - n))) & 0x07ffffff;
  r.n = ((a.n << n) | (a.n >> (27 - n))) & 0x07ffffff;
  return r;
}

static inline uint8_t t27_get(const t27_t* a, const int pos) {
  const uint32_t mask = 1 << pos;
  if (a->p & mask) {
    return 1;
  } else if (a->n & mask) {
    return 2;
  }
  return 0;
}

static inline void t27_set(t27_t* a, const int pos, const uint8_t val) {
  if (val > 2) return;
  const uint32_t mask = 1 << pos;
  const uint32_t unmask = ~mask;
  a->p = a->p & unmask;
  a->n = a->n & unmask;
  if (val == 1) {
    a->p |= mask;
  } else if (val == 2) {
    a->n |= mask;
  } else {
    return;
  }
}

static inline void state_to_fstate(trit_t* state, t27_t* fstate) {
  for (size_t slice = 0; slice < T27_NUM_SLICES; ++slice) {
    for (size_t row = 0; row < T27_NUM_ROWS; ++row) {
      for (size_t colum = 0; colum < T27_NUM_COLUMNS; ++colum) {
        trit_t t = state[T27_SLICE_SIZE * slice + T27_NUM_COLUMNS * row + colum];
        t27_set(&fstate[T27_NUM_COLUMNS * row + colum], slice, t);
      }
    }
  }
}

static inline void fstate_to_state(t27_t* fstate, trit_t* state) {
  for (size_t slice = 0; slice < T27_NUM_SLICES; ++slice) {
    for (size_t row = 0; row < T27_NUM_ROWS; ++row) {
      for (size_t colum = 0; colum < T27_NUM_COLUMNS; ++colum) {
        state[T27_SLICE_SIZE * slice + T27_NUM_COLUMNS * row + colum] =
            t27_get(&fstate[T27_NUM_COLUMNS * row + colum], slice);
      }
    }
  }
}

#endif  //__COMMON_FTROIKA_T27_H__
