/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "common/curl-p/hashcash.h"
#include "common/curl-p/ptrit.h"
#include "common/curl-p/search.h"

#ifdef _WIN32
#define CTZLL(x) _tzcnt_u64(x)
#else
#define CTZLL(x) __builtin_ctzll(x)
#endif

short test(PCurl *const curl, unsigned short const mwm) {
  unsigned short i;
  ptrit_s probe = HIGH_BITS;
  for (i = HASH_LENGTH_TRIT; i-- > HASH_LENGTH_TRIT - mwm && probe != 0;) {
    probe &= ~(curl->state[i].low ^ curl->state[i].high);
  }
  if (probe == 0) {
    return -1;
  }
  return CTZLL(probe);
}

#undef CTZLL

PearlDiverStatus hashcash(Curl *const ctx, SearchType const type,
                          unsigned short const offset, unsigned short const end,
                          unsigned short const min_weight) {
  return pd_search(ctx, offset, end, &test, min_weight);
}
