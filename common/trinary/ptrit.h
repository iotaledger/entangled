/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_TRINARY_PTRIT_H_
#define __COMMON_TRINARY_PTRIT_H_

#include "common/stdint.h"

extern uint64_t const HIGH_BITS;
extern uint64_t const LOW_BITS;

typedef uint64_t ptrit_s;

typedef struct {
  ptrit_s low;
  ptrit_s high;
} ptrit_t;

#endif
