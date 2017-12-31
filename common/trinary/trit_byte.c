#include "trit_byte.h"

#define NUMBER_OF_TRITS_IN_A_BYTE 5
#define RADIX 3
const byte_t byte_radix[] = {1, 3, 9, 27, 81};

size_t min_bytes(size_t);
byte_t trits_to_byte(trit_t *, byte_t, size_t);
void byte_to_trits(byte_t byte, trit_t *trit, size_t i);

void trits_to_bytes(trit_t *trits, byte_t *bytes, size_t size) {
  if (size <= 0) {
    return;
  }
  size_t end;
  if (size < NUMBER_OF_TRITS_IN_A_BYTE) {
    end = size;
  } else {
    end = NUMBER_OF_TRITS_IN_A_BYTE;
  }
  bytes[0] = trits_to_byte(trits, 0, end - 1);
  trits_to_bytes(&trits[end], &bytes[1], size - end);
}

size_t min_bytes(size_t trits_length) {
  return (trits_length + NUMBER_OF_TRITS_IN_A_BYTE - 1) /
         NUMBER_OF_TRITS_IN_A_BYTE;
}

byte_t trits_to_byte(trit_t *trits, byte_t cum, size_t i) {
  if (i == 0) {
    return cum * RADIX + trits[i];
  }
  return trits_to_byte(trits, cum * RADIX + trits[i], i - 1);
}

void bytes_to_trits(byte_t *byte, size_t n_bytes, trit_t *trit,
                    size_t n_trits) {
  if (n_bytes == 0 || n_trits == 0) {
    return;
  }
  size_t end =
      n_trits < NUMBER_OF_TRITS_IN_A_BYTE ? n_trits : NUMBER_OF_TRITS_IN_A_BYTE;
  byte_to_trits(*byte, trit, end - 1);
  bytes_to_trits(&byte[1], n_bytes - 1, &trit[end], n_trits - end);
}

void byte_to_trits(byte_t byte, trit_t *trit, size_t i) {
  if (byte > (byte_radix[i] >> 1)) {
    byte -= byte_radix[i];
    trit[i] = 1;
  } else if (byte < -(byte_radix[i] >> 1)) {
    byte += byte_radix[i];
    trit[i] = -1;
  } else {
    trit[i] = 0;
  }
  if (i == 0) {
    return;
  }
  byte_to_trits(byte, trit, i - 1);
}
