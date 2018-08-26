/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "bct.h"

enum { WORD_SIZE = sizeof(bct_t) * 4 };

static int tindex(int const b) { return b / WORD_SIZE; }
static int toffset(int const b) { return (b % WORD_SIZE) << 1; }
/*
   inline int tindex(int b) { return b / WORD_SIZE; }
   inline int toffset(int b) { return b % WORD_SIZE; }
   */

void copy_bct(bct_t *const d, size_t const o, bct_t const *const s,
              size_t const i, size_t const l) {
  if (l == 0) {
    return;
  }
  write_trit(d, o, get_trit(s, i));
  copy_bct(d, o + 1, s, i + 1, l - 1);
}

void copy_trits_to_bct(bct_t *const s, size_t const i, trit_t const *const t,
                       size_t const l) {
  if (l == 0) {
    return;
  }
  write_trit(s, i, *t);
  copy_trits_to_bct(s, i + 1, &t[1], l - 1);
}

void copy_bct_to_trits(trit_t *const t, bct_t const *const s, size_t const i,
                       size_t const l) {
  if (l == 0) {
    return;
  }
  *t = get_trit(s, i);
  copy_bct_to_trits(&t[1], s, i + 1, l - 1);
}

void write_trit(bct_t *const data, int const t, trit_t const v) {
  switch (v) {
    case 1:
      data[tindex(t)] &= ~(1 << toffset(t));
      data[tindex(t)] |= 1 << (toffset(t) + 1);
      break;
    case -1:
      data[tindex(t)] |= 1 << toffset(t);
      data[tindex(t)] &= ~(1 << (toffset(t) + 1));
      break;
    default:
      data[tindex(t)] |= 1 << toffset(t);
      data[tindex(t)] |= 1 << (toffset(t) + 1);
  }
}

trit_t get_trit(bct_t const *const data, int const t) {
  if (data[tindex(t)] & (1 << (toffset(t)))) {
    if (data[tindex(t)] & (1 << (toffset(t) + 1))) {
      return 0;
    }
    return -1;
  }
  return 1;
}
