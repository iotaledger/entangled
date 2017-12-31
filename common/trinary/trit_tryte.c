#include <string.h>

#include "trit_tryte.h"

#define NUMBER_OF_TRITS_IN_A_TRYTE 3
#define TRYTE_SPACE 27
#define TRYTE_STRING "9ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define TRITS_TO_TRYTES_MAP                                                 \
  {0, 0, 0}, {1, 0, 0}, {-1, 1, 0}, {0, 1, 0}, {1, 1, 0}, {-1, -1, 1},      \
      {0, -1, 1}, {1, -1, 1}, {-1, 0, 1}, {0, 0, 1}, {1, 0, 1}, {-1, 1, 1}, \
      {0, 1, 1}, {1, 1, 1}, {-1, -1, -1}, {0, -1, -1}, {1, -1, -1},         \
      {-1, 0, -1}, {0, 0, -1}, {1, 0, -1}, {-1, 1, -1}, {0, 1, -1},         \
      {1, 1, -1}, {-1, -1, 0}, {0, -1, 0}, {1, -1, 0}, {                    \
    -1, 0, 0                                                                \
  }

const trit_t TRYTE_TO_TRITS_MAPPINGS[TRYTE_SPACE][NUMBER_OF_TRITS_IN_A_TRYTE] =
    {TRITS_TO_TRYTES_MAP};

void trits_to_trytes(trit_t *trits, tryte_t *trytes, size_t length) {
  int j = 0,
      end = length < NUMBER_OF_TRITS_IN_A_TRYTE ? length
                                                : NUMBER_OF_TRITS_IN_A_TRYTE,
      i = end;
  for (; i-- > 0;) {
    j *= 3;
    j += trits[i];
  }
  // int j = trits[0] + trits[1] * 3 + trits[2] * 9;
  if (j < 0) {
    j += TRYTE_SPACE;
  }
  trytes[0] = TRYTE_STRING[(size_t)j];
  if (length < NUMBER_OF_TRITS_IN_A_TRYTE) {
    return;
  }
  trits_to_trytes(&trits[NUMBER_OF_TRITS_IN_A_TRYTE], &trytes[1],
                  length - NUMBER_OF_TRITS_IN_A_TRYTE);
}

void trytes_to_trits(tryte_t *tryte, trit_t *trits, size_t length) {
  if (length == 0) {
    return;
  }
  memcpy(trits,
         TRYTE_TO_TRITS_MAPPINGS[strchr(TRYTE_STRING, tryte[0]) - TRYTE_STRING],
         NUMBER_OF_TRITS_IN_A_TRYTE * sizeof(trit_t));
  trytes_to_trits(&tryte[1], &trits[NUMBER_OF_TRITS_IN_A_TRYTE], length - 1);
}
