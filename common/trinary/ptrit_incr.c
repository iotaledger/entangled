/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ptrit_incr.h"

#define LOW_0 0xDB6DB6DB6DB6DB6D
#define HIGH_0 0xB6DB6DB6DB6DB6DB
#define LOW_1 0xF1F8FC7E3F1F8FC7
#define HIGH_1 0x8FC7E3F1F8FC7E3F
#define LOW_2 0x7FFFE00FFFFC01FF
#define HIGH_2 0xFFC01FFFF803FFFF
#define LOW_3 0xFFC0000007FFFFFF
#define HIGH_3 0x003FFFFFFFFFFFFF

void ptrit_offset(ptrit_t *const trits, size_t const length) {
  if (length < 4) {
    return;
  }
  trits->low = LOW_0;
  trits->high = HIGH_0;
  trits[1].low = LOW_1;
  trits[1].high = HIGH_1;
  trits[2].low = LOW_2;
  trits[2].high = HIGH_2;
  trits[3].low = LOW_3;
  trits[3].high = HIGH_3;
}

void ptrit_increment(ptrit_t *const trits, size_t const offset,
                     size_t const end) {
  size_t i;
  ptrit_s carry = 1;
  ptrit_t copy;
  for (i = offset; i < end && carry != 0; i++) {
    copy.low = trits[i].low;
    copy.high = trits[i].high;
    trits[i].low = copy.high ^ copy.low;
    trits[i].high = copy.low;
    carry = copy.high & (~copy.low);
  }
}
