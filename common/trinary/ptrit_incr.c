/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ptrit_incr.h"

#include <string.h>

#if (64 == PTRIT_SIZE)
#if defined(PTRIT_CVT_ANDN)
uint64_t const ptrit_iota[PTRIT_SIZE_LOG3][2][1] = {
    {
        {
            0xb6db6db6db6db6db,
        },
        {
            0x6db6db6db6db6db6,
        },
    },
    {
        {
            0x8fc7e3f1f8fc7e3f,
        },
        {
            0x7e3f1f8fc7e3f1f8,
        },
    },
    {
        {
            0xffc01ffff803ffff,
        },
        {
            0x803ffff007fffe00,
        },
    },
    {
        {
            0x003fffffffffffff,
        },
        {
            0xfffffffff8000000,
        },
    },
};
#elif defined(PTRIT_CVT_ORN)
uint64_t const ptrit_iota[PTRIT_SIZE_LOG3][2][1] = {
    {
        {
            0x4924924924924924,
        },
        {
            0x6db6db6db6db6db6,
        },
    },
    {
        {
            0x70381c0e070381c0,
        },
        {
            0x7e3f1f8fc7e3f1f8,
        },
    },
    {
        {
            0x003fe00007fc0000,
        },
        {
            0x803ffff007fffe00,
        },
    },
    {
        {
            0xffc0000000000000,
        },
        {
            0xfffffffff8000000,
        },
    },
};
#else
#error Invalid PTRIT_CVT
#endif  // PTRIT_CVT

#elif (128 == PTRIT_SIZE)
#if defined(PTRIT_CVT_ANDN)
uint64_t const ptrit_iota[PTRIT_SIZE_LOG3][2][2] = {
    {
        {
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
        },
        {
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
        },
    },
    {
        {
            0x8fc7e3f1f8fc7e3f,
            0xc7e3f1f8fc7e3f1f,
        },
        {
            0x7e3f1f8fc7e3f1f8,
            0x3f1f8fc7e3f1f8fc,
        },
    },
    {
        {
            0xffc01ffff803ffff,
            0x3ffff007fffe00ff,
        },
        {
            0x803ffff007fffe00,
            0xffe00ffffc01ffff,
        },
    },
    {
        {
            0x003fffffffffffff,
            0xfffffffffffe0000,
        },
        {
            0xfffffffff8000000,
            0xfffff0000001ffff,
        },
    },
    {
        {
            0xffffffffffffffff,
            0xffffffffffffffff,
        },
        {
            0x0000000000000000,
            0xfffffffffffe0000,
        },
    },
};
#else
#error Invalid PTRIT_CVT
#endif  // PTRIT_CVT

#elif (256 == PTRIT_SIZE)
#if defined(PTRIT_CVT_ANDN)
uint64_t const ptrit_iota[PTRIT_SIZE_LOG3][2][4] = {
    {
        {
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
        },
        {
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
        },
    },
    {
        {
            0x8fc7e3f1f8fc7e3f,
            0xc7e3f1f8fc7e3f1f,
            0xe3f1f8fc7e3f1f8f,
            0xf1f8fc7e3f1f8fc7,
        },
        {
            0x7e3f1f8fc7e3f1f8,
            0x3f1f8fc7e3f1f8fc,
            0x1f8fc7e3f1f8fc7e,
            0x8fc7e3f1f8fc7e3f,
        },
    },
    {
        {
            0xffc01ffff803ffff,
            0x3ffff007fffe00ff,
            0xe00ffffc01ffff80,
            0xfff803ffff007fff,
        },
        {
            0x803ffff007fffe00,
            0xffe00ffffc01ffff,
            0x1ffff803ffff007f,
            0xf007fffe00ffffc0,
        },
    },
    {
        {
            0x003fffffffffffff,
            0xfffffffffffe0000,
            0xfffffffc0000007f,
            0xfff8000000ffffff,
        },
        {
            0xfffffffff8000000,
            0xfffff0000001ffff,
            0xe0000003ffffffff,
            0x0007ffffffffffff,
        },
    },
    {
        {
            0xffffffffffffffff,
            0xffffffffffffffff,
            0x00000003ffffffff,
            0xfff8000000000000,
        },
        {
            0x0000000000000000,
            0xfffffffffffe0000,
            0xffffffffffffffff,
            0x0007ffffffffffff,
        },
    },
    {
        {
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
        },
        {
            0x0000000000000000,
            0x0000000000000000,
            0x0000000000000000,
            0xfff8000000000000,
        },
    },
};
#else
#error Invalid PTRIT_CVT
#endif  // PTRIT_CVT

#elif (512 == PTRIT_SIZE)
#if defined(PTRIT_CVT_ANDN)
uint64_t const ptrit_iota[PTRIT_SIZE_LOG3][2][8] = {
    {
        {
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
        },
        {
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
            0xdb6db6db6db6db6d,
            0x6db6db6db6db6db6,
            0xb6db6db6db6db6db,
        },
    },
    {
        {
            0x8fc7e3f1f8fc7e3f,
            0xc7e3f1f8fc7e3f1f,
            0xe3f1f8fc7e3f1f8f,
            0xf1f8fc7e3f1f8fc7,
            0xf8fc7e3f1f8fc7e3,
            0xfc7e3f1f8fc7e3f1,
            0x7e3f1f8fc7e3f1f8,
            0x3f1f8fc7e3f1f8fc,
        },
        {
            0x7e3f1f8fc7e3f1f8,
            0x3f1f8fc7e3f1f8fc,
            0x1f8fc7e3f1f8fc7e,
            0x8fc7e3f1f8fc7e3f,
            0xc7e3f1f8fc7e3f1f,
            0xe3f1f8fc7e3f1f8f,
            0xf1f8fc7e3f1f8fc7,
            0xf8fc7e3f1f8fc7e3,
        },
    },
    {
        {
            0xffc01ffff803ffff,
            0x3ffff007fffe00ff,
            0xe00ffffc01ffff80,
            0xfff803ffff007fff,
            0x07fffe00ffffc01f,
            0xfc01ffff803ffff0,
            0xffff007fffe00fff,
            0x00ffffc01ffff803,
        },
        {
            0x803ffff007fffe00,
            0xffe00ffffc01ffff,
            0x1ffff803ffff007f,
            0xf007fffe00ffffc0,
            0xfffc01ffff803fff,
            0x03ffff007fffe00f,
            0xfe00ffffc01ffff8,
            0xffff803ffff007ff,
        },
    },
    {
        {
            0x003fffffffffffff,
            0xfffffffffffe0000,
            0xfffffffc0000007f,
            0xfff8000000ffffff,
            0x000001ffffffffff,
            0x03fffffffffffff0,
            0xffffffffffe00000,
            0xffffffc0000007ff,
        },
        {
            0xfffffffff8000000,
            0xfffff0000001ffff,
            0xe0000003ffffffff,
            0x0007ffffffffffff,
            0xffffffffffffc000,
            0xffffffff8000000f,
            0xffff0000001fffff,
            0x0000003fffffffff,
        },
    },
    {
        {
            0xffffffffffffffff,
            0xffffffffffffffff,
            0x00000003ffffffff,
            0xfff8000000000000,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0x00000000001fffff,
            0xffffffc000000000,
        },
        {
            0x0000000000000000,
            0xfffffffffffe0000,
            0xffffffffffffffff,
            0x0007ffffffffffff,
            0x0000000000000000,
            0xfffffffffffffff0,
            0xffffffffffffffff,
            0x0000003fffffffff,
        },
    },
    {
        {
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0x0000003fffffffff,
        },
        {
            0x0000000000000000,
            0x0000000000000000,
            0x0000000000000000,
            0xfff8000000000000,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
            0xffffffffffffffff,
        },
    },
};
#else
#error Invalid PTRIT_CVT
#endif  // PTRIT_CVT

#else
#error Invalid PTRIT_SIZE.
#endif  // PTRIT_SIZE

void ptrits_set_iota(ptrit_t range[PTRIT_SIZE_LOG3]) { memcpy(range, ptrit_iota, sizeof(ptrit_iota)); }

// Homogenous ptrits increment
// Invariant:
//   * for each ptrit `t` in [`begin`, `end`) all non-`NaT` trits in `t` are equal;
//   * all ptrits in [`begin`, `end`) contain `NaT`s in the same position;
//   * `NaT` trits are preserved after increment.
// Return:
//   * 1 if overflow;
//   * 0 if no overflow.
int ptrit_hincr(ptrit_t *const begin, ptrit_t *const end) {
  ptrit_s a, carry;
  ptrit_t *t;
  for (t = begin; t < end; ++t) {
#if defined(PTRIT_CVT_ANDN)
    // -1 -> (1,0); 0 -> (1,1); +1 -> (0,1); NaT -> (0,0)
    //   t    t+1  carry
    // (1,0) (1,1)  0
    // (1,1) (0,1)  0
    // (0,1) (1,0)  1
    // (0,0) (0,0)  0
    a = t->low;
    t->low = XOR(t->low, t->high);
    t->high = a;
    carry = ANDN(a, t->low);
#elif defined(PTRIT_CVT_ORN)
    // -1 -> (0,0); 0 -> (0,1); +1 -> (1,1); NaT -> (1,0)
    //   t    t+1  carry
    // (0,0) (0,1)  0
    // (0,1) (1,1)  0
    // (1,1) (0,0)  1
    // (1,0) (1,0)  0
    a = t->low;
    t->low = XOR(t->low, t->high);
    carry = AND(a, t->high);
    t->high = NOT(a);
#else
#error Invalid PTRIT_CVT.
#endif  // PTRIT_CVT
    if (0 == *(uint64_t *)&carry) break;
  }

  return (end == t) ? 1 : 0;
}

void ptrit_increment(ptrit_t *const trits, size_t const offset, size_t const end) {
  int carry = ptrit_hincr(trits + offset, trits + end);
  (void)carry;  // TODO: check for carry in pearl diver
}
