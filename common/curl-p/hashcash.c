#include <stdio.h>

#include "hashcash.h"
#include "ptrit.h"
#include "search.h"

short test(PCurl *curl, unsigned short mwm) {
  unsigned short i;
  ptrit_s probe = HIGH_BITS;
  for (i = HASH_LENGTH; i-- > HASH_LENGTH - mwm && probe != 0;) {
    probe &= ~(curl->state[i].low ^ curl->state[i].high);
  }
  if (probe == 0) {
    return -1;
  }
  return __builtin_ctzll(probe);
}

PearlDiverStatus hashcash(Curl *const ctx, SearchType type,
                          unsigned short const offset, unsigned short const end,
                          unsigned short const min_weight) {
  return pd_search(ctx, offset, end, &test, min_weight);
}
