/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include "common/defs.h"
#include "common/kerl/bigint.h"
#include "common/kerl/converter.h"

#define INT_LEN 12
#define BYTE_LEN 48
#define BYTE_LEN_2 24
#define TRIT_LEN 243

static const uint32_t HALF_3[] = {
    0xa5ce8964, 0x9f007669, 0x1484504f, 0x3ade00d9, 0x0c24486e, 0x50979d57,
    0x79a4c702, 0x48bbae36, 0xa9f6808b, 0xaa06a805, 0xa87fabdf, 0x5e69ebef,
};

uint8_t is_null(uint32_t const *const base) {
  size_t i = 0;
  for (; i < INT_LEN; i++) {
    if (base[i]) {
      return 0;
    }
  }

  return 1;
}

void space_reverse(uint8_t *const bytes) {
  size_t i = 0;
  uint8_t tmp;

  for (; i < BYTE_LEN_2; i++) {
    tmp = bytes[i];
    bytes[i] = bytes[BYTE_LEN - i - 1];
    bytes[BYTE_LEN - i - 1] = tmp;
  }
}

void convert_trits_to_bytes(trit_t const *const trits, uint8_t *const bytes) {
  size_t i = 0, j = 0;
  size_t sz = 1, size = 1;
  uint8_t all_minus_1 = 1;
  uint32_t carry;
  uint32_t *base = (uint32_t *)bytes;
  uint64_t v;

  memset(base, 0, INT_LEN);

  for (i = 0; i < TRIT_LEN - 1; i++) {
    if (trits[i] != -1) {
      all_minus_1 = 0;
      break;
    }
  }

  if (all_minus_1) {
    memcpy(base, HALF_3, INT_LEN * sizeof(uint32_t));
    bigint_not(base, INT_LEN);
    bigint_add_small(base, 1);
  } else {
    for (i = (TRIT_LEN - 1); i-- > 0;) {
      // multiply by radix
      {
        sz = size;
        carry = 0;

        for (j = 0; j < sz; j++) {
          v = ((uint64_t)base[j]) * ((uint64_t)RADIX) + ((uint64_t)carry);
          carry = (v >> 32uLL);
          base[j] = (uint32_t)(v & 0xFFFFFFFFuLL);
        }

        if (carry) {
          base[sz] = carry;
          size++;
        }
      }

      // addition
      {
        sz = bigint_add_small(base, trits[i] + 1);

        if (sz > size) {
          size = sz;
        }
      }
    }

    if (!is_null(base)) {
      if (bigint_cmp(HALF_3, base, INT_LEN) <= 0) {
        // base >= HALF_3
        // just do base - HALF_3
        bigint_sub(base, HALF_3, INT_LEN);
      } else {
        uint32_t tmp[INT_LEN] = {0};

        memcpy(tmp, HALF_3, INT_LEN * sizeof(uint32_t));
        bigint_sub(tmp, base, INT_LEN);
        bigint_not(tmp, INT_LEN);
        bigint_add_small(tmp, 1);
        memcpy(base, tmp, INT_LEN * sizeof(uint32_t));
      }
    }
  }

  space_reverse(bytes);
}

void convert_bytes_to_trits(uint8_t *const bytes, trit_t *const trits) {
  size_t i = 0, j = 0;
  uint8_t flip_trits = 0;
  uint64_t lhs, rem;
  uint64_t rhs = RADIX;
  uint32_t *base = (uint32_t *)bytes;

  if (is_null(base)) {
    memset(trits, 0, TRIT_LEN);
    return;
  }

  trits[TRIT_LEN - 1] = 0;
  space_reverse(bytes);

  if (!(base[INT_LEN - 1] >> 31)) {
    // positive number
    // we need to add HALF_3 to move it into positive unsigned space
    bigint_add(base, HALF_3, INT_LEN);
  } else {
    // negative number
    bigint_not(base, INT_LEN);
    if (bigint_cmp(base, HALF_3, INT_LEN) > 0) {
      bigint_sub(base, HALF_3, INT_LEN);
      flip_trits = 1;
    } else {
      bigint_add_small(base, 1);
      uint32_t tmp[INT_LEN] = {0};
      memcpy(tmp, HALF_3, INT_LEN * sizeof(uint32_t));
      bigint_sub(tmp, base, INT_LEN);
      memcpy(base, tmp, INT_LEN * sizeof(uint32_t));
    }
  }

  for (; i < TRIT_LEN - 1; i++) {
    rem = 0;
    for (j = INT_LEN; j-- > 0;) {
      lhs = (rem << 32) | base[j];
      base[j] = (uint32_t)(lhs / rhs);
      rem = (uint32_t)(lhs % rhs);
    }
    trits[i] = ((uint8_t)rem) - 1;
  }

  if (flip_trits) {
    for (i = 0; i < TRIT_LEN - 1; i++) {
      trits[i] = -trits[i];
    }
  }
}

#undef INT_LEN
#undef BYTE_LEN
#undef TRIT_LEN
#undef RADIX
