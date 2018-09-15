/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "common/trinary/flex_trit.h"
#include <stdlib.h>
#include <string.h>
#include "common/trinary/trit_byte.h"

size_t flex_trits_slice(flex_trit_t *const to_flex_trits, size_t const to_len,
                        flex_trit_t const *const flex_trits, size_t const len,
                        size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits > to_len || (start + num_trits) > len) {
    return 0;
  }
  size_t num_bytes = num_flex_trits_for_trits(num_trits);
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_flex_trits, flex_trits + start, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  trit_t trits[6];
  size_t index = start / 3U;
  size_t offset = start % 3U;
  size_t end_index = (start + num_trits - 1) / 3U;
  size_t i, j, tlen = 3;
  for (i = index, j = 0; i < index + num_bytes; i++, j++) {
    trytes_to_trits(&flex_trits[i], trits, 1);
    if (i < end_index) {
      if (offset) {
        trytes_to_trits(&flex_trits[i + 1], trits + 3, 1);
      }
    } else if (num_bytes * 3 > num_trits) {
      tlen = (num_trits - num_bytes * 3 + 3);
    }
    trits_to_trytes(trits + offset, to_flex_trits + j, tlen);
  }
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  uint8_t buffer;
  uint8_t tshift = (start & 3) << 1U;
  uint8_t rshift = (8U - tshift) % 8U;
  size_t index = start >> 2U;
  size_t end_index = (start + num_trits - 1) >> 2U;
  size_t i, j;
  // Calculate the number of bytes to copy over
  for (i = index, j = 0; i < index + num_bytes; i++, j++) {
    buffer = flex_trits[i];
    buffer = buffer >> tshift;
    if (i < end_index) {
      if (rshift) {
        buffer |= (flex_trits[i + 1] << rshift);
      }
    } else {
      uint8_t residual = (num_trits & 3);
      if (residual) {
        uint8_t shift = (4 - residual) << 1U;
        buffer = (uint8_t)(buffer << shift) >> shift;
      }
    }
    to_flex_trits[j] = buffer;
  }
  // There is a risk of noise after the last trit so we need to clean up
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  trit_t trits[10] = {0};
  size_t index = start / 5U;
  size_t offset = start % 5U;
  size_t end_index = (start + num_trits - 1) / 5U;
  size_t i, j, tlen = 5;
  for (i = index, j = 0; i < index + num_bytes; i++, j++) {
    bytes_to_trits(((byte_t *)flex_trits + i), 1, trits, 5);
    if (i < end_index) {
      if (offset) {
        bytes_to_trits(((byte_t *)flex_trits + i + 1), 1, ((trit_t *)trits + 5),
                       5);
      }
    } else if (num_bytes * 5 > num_trits) {
      tlen = (num_trits - num_bytes * 5 + 5);
    }
    to_flex_trits[j] = trits_to_byte(trits + offset, 0, tlen - 1);
  }
#endif
  return num_bytes;
}

size_t flex_trits_insert(flex_trit_t *const to_flex_trits, size_t const to_len,
                         flex_trit_t const *const flex_trits, size_t const len,
                         size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  for (size_t i = 0; i < num_trits; i++) {
    flex_trits_set_at(to_flex_trits, to_len, start + i,
                      flex_trits_at(flex_trits, len, i));
  }
  return num_trits;
}

size_t flex_trits_to_trits(trit_t *const trits, size_t const to_len,
                           flex_trit_t const *const flex_trits,
                           size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits < len || num_trits < to_len) {
    return 0;
  }
  size_t num_bytes = num_flex_trits_for_trits(num_trits);
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
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
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_flex_trits, trits, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, num_flex_trits_for_trits(to_len));
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
  memset(to_flex_trits, 0, num_flex_trits_for_trits(to_len));
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
