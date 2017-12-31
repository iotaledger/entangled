#include "bct.h"

enum { WORD_SIZE = sizeof(bct_t) * 4 };

int tindex(int b) { return b / WORD_SIZE; }
int toffset(int b) { return (b % WORD_SIZE) << 1; }
/*
   inline int tindex(int b) { return b / WORD_SIZE; }
   inline int toffset(int b) { return b % WORD_SIZE; }
   */

void copy_bct(bct_t *d, size_t o, bct_t *s, size_t i, size_t l) {
  if (l == 0) {
    return;
  }
  write_trit(d, o, get_trit(s, i));
  copy_bct(d, o + 1, s, i + 1, l - 1);
}

void copy_trits_to_bct(bct_t *s, size_t i, trit_t *t, size_t l) {
  if (l == 0) {
    return;
  }
  write_trit(s, i, *t);
  copy_trits_to_bct(s, i + 1, &t[1], l - 1);
}

void copy_bct_to_trits(trit_t *t, bct_t *s, size_t i, size_t l) {
  if (l == 0) {
    return;
  }
  *t = get_trit(s, i);
  copy_bct_to_trits(&t[1], s, i + 1, l - 1);
}

void write_trit(bct_t *data, int t, trit_t v) {
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

trit_t get_trit(bct_t *data, int t) {
  if (data[tindex(t)] & (1 << (toffset(t)))) {
    if (data[tindex(t)] & (1 << (toffset(t) + 1))) {
      return 0;
    }
    return -1;
  }
  return 1;
}
