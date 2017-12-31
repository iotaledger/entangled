#include "ptrit_incr.h"

void ptrit_increment(ptrit_t *const trits, size_t offset, size_t end) {
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
