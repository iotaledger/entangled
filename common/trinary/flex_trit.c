/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/trinary/flex_trit.h"
#include "common/trinary/trit_byte.h"
#include "utils/macros.h"

#if defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
static flex_trit_t flex_trit_set_residual(flex_trit_t flex_trit, size_t residual) {
  // residual <= 4
  size_t shift = (4 - residual) << 1U;
  flex_trit <<= shift;
  flex_trit >>= shift;
  return flex_trit;
}
#endif

size_t flex_trits_slice(flex_trit_t *const to_flex_trits, size_t const to_len, flex_trit_t const *const flex_trits,
                        size_t const len, size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > to_len || (start + num_trits) > len) {
    return 0;
  }
  size_t num_bytes = NUM_FLEX_TRITS_FOR_TRITS(num_trits);
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_FOR_TRITS(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  // num_bytes == num_trits in a 1:1 scheme
  memcpy(to_flex_trits, flex_trits + start, num_bytes);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  if (0 == (start % NUM_TRITS_PER_FLEX_TRIT)) {
    // Copy flex trits as bytes
    memcpy(to_flex_trits, flex_trits + start / NUM_TRITS_PER_FLEX_TRIT, num_trits / NUM_TRITS_PER_FLEX_TRIT);
    // Handle tail
    if (0 != num_trits % NUM_TRITS_PER_FLEX_TRIT) {
      trit_t trits[3] = {0};
      trytes_to_trits(flex_trits + (start + num_trits) / NUM_TRITS_PER_FLEX_TRIT, trits, 1);
      trits_to_trytes(trits, to_flex_trits + num_trits / NUM_TRITS_PER_FLEX_TRIT, num_trits % NUM_TRITS_PER_FLEX_TRIT);
    }
  } else {
    trit_t trits[6] = {0};
    size_t index = start / 3U;
    size_t offset = start % 3U;
    size_t i, j;
    size_t end_index = NUM_FLEX_TRITS_FOR_TRITS(len) - 1;
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
  }
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  if (0 == (start % NUM_TRITS_PER_FLEX_TRIT)) {
    // Copy flex trits as bytes
    memcpy(to_flex_trits, flex_trits + start / NUM_TRITS_PER_FLEX_TRIT, num_trits / NUM_TRITS_PER_FLEX_TRIT);
    // Handle tail
    if (0 != num_trits % NUM_TRITS_PER_FLEX_TRIT) {
      to_flex_trits[num_trits / NUM_TRITS_PER_FLEX_TRIT] = flex_trit_set_residual(
          flex_trits[(start + num_trits) / NUM_TRITS_PER_FLEX_TRIT], num_trits % NUM_TRITS_PER_FLEX_TRIT);
    }
  } else {
    uint8_t buffer = 0;
    uint8_t tshift = (start & 3) << 1U;
    uint8_t rshift = (8U - tshift) & 7;
    size_t index = start >> 2U;
    size_t i, j;
    size_t end_index = NUM_FLEX_TRITS_FOR_TRITS(len) - 1;
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
      to_flex_trits[num_bytes - 1] = flex_trit_set_residual(buffer, residual);
    }
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  if (0 == (start % NUM_TRITS_PER_FLEX_TRIT)) {
    // Copy flex trits as bytes
    memcpy(to_flex_trits, flex_trits + start / NUM_TRITS_PER_FLEX_TRIT, num_trits / NUM_TRITS_PER_FLEX_TRIT);
    // Handle tail
    if (0 != num_trits % NUM_TRITS_PER_FLEX_TRIT) {
      trit_t trits[5] = {0};
      bytes_to_trits((byte_t *)(flex_trits + (start + num_trits) / NUM_TRITS_PER_FLEX_TRIT), 1, trits, 5);
      to_flex_trits[num_trits / NUM_TRITS_PER_FLEX_TRIT] = trits_to_byte(trits, num_trits % NUM_TRITS_PER_FLEX_TRIT);
    }
  } else {
    trit_t trits[10] = {0};
    size_t index = start / 5U;
    size_t offset = start % 5U;
    size_t i, j;
    size_t end_index = NUM_FLEX_TRITS_FOR_TRITS(len) - 1;
    for (i = index, j = 0; j < num_bytes; i++, j++) {
      bytes_to_trits(((byte_t *)flex_trits + i), 1, trits, 5);
      if (offset && i < end_index) {
        bytes_to_trits(((byte_t *)flex_trits + i + 1), 1, ((trit_t *)trits + 5), 5);
      }
      to_flex_trits[j] = trits_to_byte(trits + offset, 5);
    }
    // There is a risk of noise after the last trit so we need to clean up
    uint8_t residual = num_trits % 5U;
    if (residual) {
      to_flex_trits[num_bytes - 1] = trits_to_byte(trits + offset, residual);
    }
  }
#endif
  return num_bytes;
}

size_t flex_trits_insert(flex_trit_t *const to_flex_trits, size_t const to_len, flex_trit_t const *const flex_trits,
                         size_t const len, size_t const start, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > len || num_trits > to_len || start >= to_len || (start + num_trits) > to_len) {
    return 0;
  }
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  memcpy(to_flex_trits + start, flex_trits, num_trits);
#else
  if (0 == (start % NUM_TRITS_PER_FLEX_TRIT)) {
    // Copy flex trits as bytes
    memcpy(to_flex_trits + start / NUM_TRITS_PER_FLEX_TRIT, flex_trits, num_trits / NUM_TRITS_PER_FLEX_TRIT);
    // Handle tail
    for (size_t i = (num_trits / NUM_TRITS_PER_FLEX_TRIT) * NUM_TRITS_PER_FLEX_TRIT; i < num_trits; i++) {
      flex_trits_set_at(to_flex_trits, to_len, start + i, flex_trits_at(flex_trits, len, i));
    }
  } else {
    for (size_t i = 0; i < num_trits; i++) {
      flex_trits_set_at(to_flex_trits, to_len, start + i, flex_trits_at(flex_trits, len, i));
    }
  }
#endif
  return num_trits;
}

size_t flex_trits_insert_from_pos(flex_trit_t *const dst_trits, size_t const dst_len,
                                  flex_trit_t const *const src_trits, size_t const src_len, size_t const src_start_pos,
                                  size_t const dst_start_pos, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > src_len || num_trits > dst_len || src_start_pos >= src_len ||
      (src_start_pos + num_trits) > src_len || dst_start_pos >= dst_len || (dst_start_pos + num_trits) > dst_len) {
    return 0;
  }
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  memcpy(dst_trits + dst_start_pos, src_trits + src_start_pos, num_trits);
#else
  if (num_trits >= NUM_TRITS_PER_FLEX_TRIT &&
      (src_start_pos % NUM_TRITS_PER_FLEX_TRIT) == (dst_start_pos % NUM_TRITS_PER_FLEX_TRIT)) {
    // Handle head
    size_t const head_num_trits = NUM_TRITS_PER_FLEX_TRIT - (src_start_pos % NUM_TRITS_PER_FLEX_TRIT);
    for (size_t i = 0; i < head_num_trits; i++) {
      trit_t t = flex_trits_at(src_trits, src_len, src_start_pos + i);
      flex_trits_set_at(dst_trits, dst_len, dst_start_pos + i, t);
    }
    // Copy flex trits as bytes
    size_t const body_num_bytes = (num_trits - head_num_trits) / NUM_TRITS_PER_FLEX_TRIT;
    memcpy(dst_trits + (dst_start_pos + NUM_TRITS_PER_FLEX_TRIT - 1) / NUM_TRITS_PER_FLEX_TRIT,
           src_trits + (src_start_pos + NUM_TRITS_PER_FLEX_TRIT - 1) / NUM_TRITS_PER_FLEX_TRIT, body_num_bytes);
    // Handle tail
    for (size_t i = head_num_trits + body_num_bytes * NUM_TRITS_PER_FLEX_TRIT; i < num_trits; i++) {
      trit_t t = flex_trits_at(src_trits, src_len, src_start_pos + i);
      flex_trits_set_at(dst_trits, dst_len, dst_start_pos + i, t);
    }
  } else {
    for (size_t i = 0; i < num_trits; i++) {
      trit_t t = flex_trits_at(src_trits, src_len, src_start_pos + i);
      flex_trits_set_at(dst_trits, dst_len, dst_start_pos + i, t);
    }
  }
#endif
  return num_trits;
}

size_t flex_trits_to_trits(trit_t *const trits, size_t const to_len, flex_trit_t const *const flex_trits,
                           size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits == 0 || num_trits > len || num_trits > to_len) {
    return 0;
  }
  size_t num_bytes = NUM_FLEX_TRITS_FOR_TRITS(num_trits);
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

size_t flex_trits_from_trits(flex_trit_t *const to_flex_trits, size_t const to_len, trit_t const *const trits,
                             size_t const len, size_t const num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len || num_trits == 0) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_FOR_TRITS(to_len));
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

size_t flex_trits_to_trytes(tryte_t *trytes, size_t to_len, const flex_trit_t *flex_trits, size_t len,
                            size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len * 3) {
    return 0;
  }
  memset(trytes, '9', to_len);
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trits_to_trytes((trit_t *)flex_trits, trytes, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  uint8_t residual = num_trits % 3U;
  size_t num_bytes = NUM_FLEX_TRITS_FOR_TRITS(num_trits);
  // There is a risk of noise after the last trit so we need to clean up
  if (residual) {
    trit_t trits[3];
    num_bytes--;
    trytes_to_trits((tryte_t *)(flex_trits + num_bytes), trits, 1);
    trits_to_trytes(trits, (tryte_t *)(trytes + num_bytes), residual);
  }
  memcpy(trytes, flex_trits, num_bytes);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE) || defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t max_trits, trits_for_tryte, trits_counter = num_trits;
  for (int i = 0, j = 0; trits_counter || offset; j++) {
    if (offset < 3) {
      max_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, trits_counter);
      flex_trits_to_trits(shifter.trits + offset, max_trits, &flex_trits[i++], max_trits, max_trits);
      trits_counter -= max_trits;
      offset += max_trits;
    }
    trits_for_tryte = MIN(3, offset);
    trits_to_trytes(shifter.trits, &trytes[j], trits_for_tryte);
#if BYTE_ORDER == LITTLE_ENDIAN
    shifter.val = shifter.val >> 24;
#elif BYTE_ORDER == BIG_ENDIAN
    shifter.val = shifter.val << 24;
#endif
    offset -= trits_for_tryte;
  }
#endif
  return num_trits;
}

size_t flex_trits_from_trytes(flex_trit_t *to_flex_trits, size_t to_len, const tryte_t *trytes, size_t len,
                              size_t num_trytes) {
  // Bounds checking
  if (num_trytes > len || num_trytes * 3 > to_len) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_FOR_TRITS(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trytes_to_trits((tryte_t *)trytes, to_flex_trits, num_trytes);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE)
  memcpy(to_flex_trits, trytes, num_trytes);
#elif defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE) || defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t num_trits, n_trytes = num_trytes;
  for (int i = 0, j = 0; n_trytes || offset; i++, j++) {
    if (n_trytes) {
      trytes_to_trits(&trytes[i], shifter.trits + offset, 1);
      n_trytes -= 1;
      offset += 3;
      if (offset < NUM_TRITS_PER_FLEX_TRIT && n_trytes) {
        i++;
        trytes_to_trits(&trytes[i], shifter.trits + offset, 1);
        n_trytes -= 1;
        offset += 3;
      }
    }
    num_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, offset);
    flex_trits_from_trits(to_flex_trits + j, num_trits, shifter.trits, num_trits, num_trits);
#if BYTE_ORDER == LITTLE_ENDIAN
    shifter.val = shifter.val >> (num_trits << 3);
#elif BYTE_ORDER == BIG_ENDIAN
    shifter.val = shifter.val << (num_trits << 3);
#endif
    offset -= num_trits;
  }
#endif
  return num_trytes;
}

size_t flex_trits_to_bytes(byte_t *bytes, size_t to_len, const flex_trit_t *flex_trits, size_t len, size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  memset(bytes, 0, MIN_BYTES(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  trits_to_bytes((trit_t *)flex_trits, bytes, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE) || defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t max_trits, trits_for_byte, trits_counter = num_trits;
  for (int i = 0, j = 0; trits_counter; i++, j++) {
    max_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, trits_counter);
    flex_trits_to_trits(shifter.trits + offset, max_trits, &flex_trits[i], max_trits, max_trits);
    trits_counter -= max_trits;
    offset += max_trits;
    if (offset < 5 && trits_counter) {
      i++;
      max_trits = MIN(NUM_TRITS_PER_FLEX_TRIT, trits_counter);
      flex_trits_to_trits(shifter.trits + offset, max_trits, &flex_trits[i], max_trits, max_trits);
      trits_counter -= max_trits;
      offset += max_trits;
    }
    trits_for_byte = MIN(5, offset);
    bytes[j] = trits_to_byte(shifter.trits, trits_for_byte);
#if BYTE_ORDER == LITTLE_ENDIAN
    shifter.val = shifter.val >> 40;
#elif BYTE_ORDER == BIG_ENDIAN
    shifter.val = shifter.val << 40;
#endif
    offset -= 5;
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  size_t num_bytes = MIN_BYTES(num_trits);
  size_t residual = num_trits % 5;
  if (residual) {
    trit_t last_byte[5] = {0};
    num_bytes--;
    byte_to_trits(flex_trits[num_bytes], last_byte, 5);
    bytes[num_bytes] = trits_to_byte(last_byte, residual);
  }
  memcpy(bytes, flex_trits, num_bytes);
#endif
  return num_trits;
}

size_t flex_trits_from_bytes(flex_trit_t *to_flex_trits, size_t to_len, const byte_t *bytes, size_t len,
                             size_t num_trits) {
  // Bounds checking
  if (num_trits > len || num_trits > to_len) {
    return 0;
  }
  memset(to_flex_trits, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_FOR_TRITS(to_len));
#if defined(FLEX_TRIT_ENCODING_1_TRIT_PER_BYTE)
  size_t num_bytes = MIN_BYTES(num_trits);
  bytes_to_trits(bytes, num_bytes, to_flex_trits, num_trits);
#elif defined(FLEX_TRIT_ENCODING_3_TRITS_PER_BYTE) || defined(FLEX_TRIT_ENCODING_4_TRITS_PER_BYTE)
  union _shifter {
    uint64_t val;
    trit_t trits[8];
  };
  union _shifter shifter = {0};
  size_t offset = 0;
  size_t max_trits, trits_for_byte, trits_counter = num_trits;
  for (int i = 0, j = 0; trits_counter; j++) {
    max_trits = MIN(5, trits_counter);
    if (offset < NUM_TRITS_PER_FLEX_TRIT && offset < trits_counter) {
      byte_to_trits(bytes[i], shifter.trits + offset, max_trits);
      offset += max_trits;
      i++;
    }
    trits_for_byte = MIN(max_trits, NUM_TRITS_PER_FLEX_TRIT);
    flex_trits_from_trits(&to_flex_trits[j], trits_counter, shifter.trits, trits_for_byte, trits_for_byte);
    trits_counter -= trits_for_byte;
#if BYTE_ORDER == LITTLE_ENDIAN
    shifter.val = shifter.val >> (trits_for_byte << 3);
#elif BYTE_ORDER == BIG_ENDIAN
    shifter.val = shifter.val << (trits_for_byte << 3);
#endif
    offset -= trits_for_byte;
  }
#elif defined(FLEX_TRIT_ENCODING_5_TRITS_PER_BYTE)
  size_t num_bytes = MIN_BYTES(num_trits);
  size_t residual = num_trits % 5;
  if (residual) {
    trit_t last_byte[5] = {0};
    num_bytes--;
    byte_to_trits(bytes[num_bytes], last_byte, 5);
    to_flex_trits[num_bytes] = trits_to_byte(last_byte, residual);
  }
  memcpy(to_flex_trits, bytes, num_bytes);
#endif
  return num_trits;
}

void flex_trit_print(flex_trit_t const *const flex_trits, size_t trits_len) {
  size_t tryte_len = trits_len / 3;
  tryte_t tryte_buff[tryte_len + 1];
  flex_trits_to_trytes(tryte_buff, tryte_len, flex_trits, trits_len, trits_len);
  tryte_buff[tryte_len] = '\0';
  printf("%s", tryte_buff);
}
