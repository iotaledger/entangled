#include "trit_ptrit.h"

#define HIGH_BITS 0xFFFFFFFFFFFFFFFF
#define LOW_BITS 0x0000000000000000

void trits_to_ptrits(trit_t *trits, ptrit_t *ptrits, size_t length) {
  if (length == 0) {
    return;
  }
  switch (*trits) {
    case 0:
      ptrits->low = HIGH_BITS;
      ptrits->high = HIGH_BITS;
      break;
    case 1:
      ptrits->low = LOW_BITS;
      ptrits->high = HIGH_BITS;
      break;
    default:
      ptrits->low = HIGH_BITS;
      ptrits->high = LOW_BITS;
  }
  trits_to_ptrits(&trits[1], &ptrits[1], length - 1);
}

void ptrits_to_trits(ptrit_t *ptrits, trit_t *trits, size_t index,
                     size_t length) {
  if (length == 0) {
    return;
  }

  trits[0] = (ptrits->low & (1uL << index))
                 ? ((ptrits->high & (1uL << index)) ? 0 : -1)
                 : 1;
  ptrits_to_trits(&ptrits[1], &trits[1], index, length - 1);
}
