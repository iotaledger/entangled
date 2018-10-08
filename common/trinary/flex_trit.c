/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "common/trinary/flex_trit.h"
#include "common/trinary/trit_byte.h"
#include "utils/macros.h"
size_t flex_trits_slice(flex_trit_t *const to_flex_trits, size_t const to_len,
                        flex_trit_t const *const flex_trits, size_t const len,
                        size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > to_len || (start + num_trits) > len) {
    return 0;
  }
  size_t num_bytes = num_flex_trits_for_trits(num_trits);
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, num_flex_trits_for_trits(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // num_bytes == num_trits in a 1:1 scheme
  memcpy(to_flex_trits, flex_trits + start, num_bytes);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  trit_t trits[6];
  size_t index = start / 3U;
  size_t offset = start % 3U;
  size_t i, j;
  size_t end_index = num_flex_trits_for_trits(len) - 1;
  for (i = index, j = 0; j < num_bytes; i++, j++) {
    trytes_to_trits(&flex_trits[i], trits, 1);
    if (offset && i < end_index) {
      trytes_to_trits(&flex_trits[i + 1], trits + 3, 1);
    }
    trits_to_trytes(trits + offset, to_flex_trits + j, 3);
  }
  // There is a risk of noise after the last trit so we need to clean up
  uint8_t residual = num_trits % 3U;
  if (residual) {
    // size_t tlen = (num_trits - num_bytes * 3 + 3);
    trits_to_trytes(trits + offset, to_flex_trits + num_bytes - 1, residual);
  }
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  uint8_t buffer = 0;
  uint8_t tshift = (start & 3) << 1U;
  uint8_t rshift = (8U - tshift) & 7;
  size_t index = start >> 2U;
  size_t i, j;
  size_t end_index = num_flex_trits_for_trits(len) - 1;
  // Calculate the number of bytes to copy over
  for (i = index, j = 0; j < num_bytes; i++, j++) {
    buffer = flex_trits[i];
    buffer = buffer >> tshift;
    if (rshift && i < end_index) {
      buffer |= (flex_trits[i + 1] << rshift);
    }
    to_flex_trits[j] = buffer;
  }
  // There is a risk of noise after the last trit so we need to clean up
  uint8_t residual = (num_trits & 3);
  if (residual) {
    uint8_t shift = (4 - residual) << 1U;
    buffer <<= shift;
    buffer >>= shift;
    to_flex_trits[num_bytes - 1] = buffer;
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[10] = {0};
  size_t index = start / 5U;
  size_t offset = start % 5U;
  size_t i, j;
  size_t end_index = num_flex_trits_for_trits(len) - 1;
  for (i = index, j = 0; j < num_bytes; i++, j++) {
    bytes_to_trits(((byte_t *)flex_trits + i), 1, trits, 5);
    if (offset && i < end_index) {
      bytes_to_trits(((byte_t *)flex_trits + i + 1), 1, ((trit_t *)trits + 5),
                     5);
    }
    to_flex_trits[j] = trits_to_byte(trits + offset, 0, 4);
  }
  // There is a risk of noise after the last trit so we need to clean up
  uint8_t residual = num_trits % 5U;
  if (residual) {
    to_flex_trits[num_bytes - 1] =
        trits_to_byte(trits + offset, 0, residual - 1);
  }
#endif
  return num_bytes;
}

size_t flex_trits_insert(flex_trit_t *const to_flex_trits, size_t const to_len,
                         flex_trit_t const *const flex_trits, size_t const len,
                         size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > len || num_trits > to_len ||
      start >= to_len || (start + num_trits) >= to_len) {
    return 0;
  }
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_flex_trits + start, flex_trits, num_trits);
#else
  for (size_t i = 0; i < num_trits; i++) {
    flex_trits_set_at(to_flex_trits, to_len, start + i,
                      flex_trits_at(flex_trits, len, i));
  }
#endif
  return num_trits;
}

size_t flex_trits_to_trits(trit_t *const trits, size_t const to_len,
                           flex_trit_t const *const flex_trits,
                           size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > len || num_trits > to_len) {
    return 0;
  }
  size_t num_bytes = num_flex_trits_for_trits(num_trits);
  memset(trits, 0, to_len);
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // num_bytes == num_trits in a 1:1 scheme
  memcpy(trits, flex_trits, num_bytes);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  trytes_to_trits(flex_trits, trits, num_bytes);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  for (size_t i = 0; i < num_trits; i++) {
    trits[i] = flex_trits_at(flex_trits, len, i);
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  bytes_to_trits((byte_t const *const)flex_trits, num_bytes, trits, num_trits);
#endif
  return num_bytes;
}

size_t flex_trits_from_trits(flex_trit_t *const to_flex_trits,
                             size_t const to_len, trit_t const *const trits,
                             size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, num_flex_trits_for_trits(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // num_bytes == num_trits in a 1:1 scheme
  memcpy(to_flex_trits, trits, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  trits_to_trytes(trits, to_flex_trits, num_trits);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  for (size_t i = 0; i < num_trits; i++) {
    flex_trits_set_at(to_flex_trits, to_len, i, trits[i]);
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trits_to_bytes((trit_t *)trits, to_flex_trits, num_trits);
#endif
  return num_trits;
}

size_t flex_trits_to_trytes(tryte_t *trytes, size_t to_len,
                            const flex_trit_t *flex_trits, size_t len,
                            size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len * 3) {
    return 0;
  }
  memset(trytes, '9', to_len);
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trits_to_trytes((trit_t *)flex_trits, trytes, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  memcpy(trytes, flex_trits, num_flex_trits_for_trits(num_trits));
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE) || \
    defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[num_trits];
  flex_trits_to_trits(trits, num_trits, flex_trits, len, num_trits);
  trits_to_trytes((trit_t *)trits, trytes, num_trits);
#endif
  return num_trits;
}

size_t flex_trits_from_trytes(flex_trit_t *to_flex_trits, size_t to_len,
                              const tryte_t *trytes, size_t len,
                              size_t num_trytes) {
  // Bounds checking
  if (num_trytes > len || num_trytes * 3 > to_len) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, num_flex_trits_for_trits(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trytes_to_trits((tryte_t *)trytes, to_flex_trits, num_trytes);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  memcpy(to_flex_trits, trytes, num_trytes);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE) || \
    defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[to_len];
  memset(trits, 0, to_len);
  trytes_to_trits((tryte_t *)trytes, trits, num_trytes);
  flex_trits_from_trits(to_flex_trits, to_len, trits, to_len, to_len);
#endif
  return num_trytes;
}

size_t flex_trits_to_bytes(byte_t *bytes, size_t to_len,
                           const flex_trit_t *flex_trits, size_t len,
                           size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  memset(bytes, 0, min_bytes(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trits_to_bytes((trit_t *)flex_trits, bytes, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE) || \
    defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t max_trits, trits_for_byte;
  for (int i = 0, j = 0; num_trits; i++, j++) {
    max_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, num_trits);
    flex_trits_to_trits(shifter.trits + offset, max_trits, &flex_trits[i],
                        max_trits, max_trits);
    num_trits -= max_trits;
    offset += max_trits;
    if (offset < 5 && num_trits) {
      i++;
      max_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, num_trits);
      flex_trits_to_trits(shifter.trits + offset, max_trits, &flex_trits[i],
                          max_trits, max_trits);
      num_trits -= max_trits;
      offset += max_trits;
    }
    trits_for_byte = MIN(4, (offset - 1));
    bytes[j] = trits_to_byte(shifter.trits, 0, trits_for_byte);
#if BYTE_ORDER == LITTLE_ENDIAN
    shifter.val = shifter.val >> 40;
#elif BYTE_ORDER == BIG_ENDIAN
    shifter.val = shifter.val << 40;
#endif
    offset -= 5;
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  size_t num_bytes = min_bytes(num_trits);
  memcpy(bytes, flex_trits, num_bytes);
  size_t residual = num_trits % 5;
  if (residual) {
    trit_t last_byte[5] = {0};
    size_t index = num_bytes - 1;
    byte_to_trits(flex_trits[index], last_byte, 4);
    bytes[index] = trits_to_byte(last_byte, 0, residual - 1);
  }
#endif
  return num_trits;
}

size_t flex_trits_from_bytes(flex_trit_t *to_flex_trits, size_t to_len,
                             const byte_t *bytes, size_t len,
                             size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, num_flex_trits_for_trits(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  size_t num_bytes = min_bytes(num_trits);
  bytes_to_trits(bytes, num_bytes, to_flex_trits, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE) || \
    defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t max_trits, trits_for_byte;
  for (int i = 0, j = 0; num_trits; j++) {
    max_trits = MIN(5, num_trits);
    if (offset < NUM_TRITS_PER_FLEX_TRIT) {
      byte_to_trits(bytes[i], shifter.trits + offset, max_trits - 1);
      offset += max_trits;
      i++;
    }
    trits_for_byte = MIN(max_trits, NUM_TRITS_PER_FLEX_TRIT);
    flex_trits_from_trits(&to_flex_trits[j], num_trits, shifter.trits,
                          trits_for_byte, trits_for_byte);
    num_trits -= trits_for_byte;  // MIN(max_trits, NUM_TRITS_PER_FLEX_TRIT);
    shifter.val = shifter.val >> (trits_for_byte << 3);
    offset -= trits_for_byte;
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  size_t num_bytes = min_bytes(num_trits);
  memcpy(to_flex_trits, bytes, num_bytes);
  size_t residual = num_trits % 5;
  if (residual) {
    trit_t last_byte[5] = {0};
    size_t index = num_bytes - 1;
    byte_to_trits(bytes[index], last_byte, 4);
    to_flex_trits[index] = trits_to_byte(last_byte, 0, residual - 1);
  }
#endif
  return num_trits;
}
