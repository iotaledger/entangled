#include "trit_long.h"

#define RADIX 3

long long trits_to_long(trit_t *trit, size_t const i) {
  long long accum = 0;
  size_t end = i;
  while (end-- > 0) {
    accum = accum * RADIX + trit[end];
  }
  return accum;
}

size_t min_trits(long long value) {
  size_t num = 1;
  long long vp = 1;
  long long v_abs = value < 0 ? -value : value;
  while (v_abs > (vp *= RADIX)) {
    num++;
  }
  return num;
}

size_t long_to_trits(long long value, trit_t *out) {
  trit_t trit;
  size_t i, size;
  char negative;

  negative = value < 0;
  size = min_trits(value);
  if (negative) {
    value = -value;
  }
  for (i = 0; i < size; i++) {
    if (value == 0) {
      break;
    }
    trit = (value + 1) % (RADIX)-1;
    if (negative) {
      trit = -trit;
    }

    out[i] = trit;
    value++;
    value /= RADIX;
  }
  return size;
}

size_t nearest_greater_power_of_three(size_t value, size_t *power) {
  size_t vp = 1;
  *power = 0;
  while (value > vp) {
    *power += 1;
    vp *= RADIX;
  }
  return vp;
}

size_t encoded_length_variables(long long value, size_t *start, size_t *power) {
  size_t size, size_size;
  size = nearest_greater_power_of_three(min_trits(value), power);
  size_size = (*power + 1) / 2;
  *start = size_size;
  return size_size + size;
}

size_t encoded_length(long long value) {
  size_t start, power;
  return encoded_length_variables(value, &start, &power);
}

// trit_t *encode_long(long long value) {
int encode_long(long long value, trit_t *out, size_t size) {
  size_t size_size, power;
  size_t end = encoded_length_variables(value, &size_size, &power);
  if (size < end) {
    goto encode_long_err;
  }
  out[size_size - 1] = power & 1 ? -1 : 1;
  long_to_trits(value, &out[size_size]);
  return 0;
encode_long_err:
  return -1;
}

long long get_encoded_long(trit_t *trits, size_t length, size_t *end) {
  size_t size = 1, i;
  for (i = 0; i < length && trits[i] == 0; i++, size *= 3) {
    size *= 3;
  }
  if (trits[i] == -1) {
    size *= 3;
  }
  i++;
  *end = size + i;
  return trits_to_long(&trits[i], size);
}
