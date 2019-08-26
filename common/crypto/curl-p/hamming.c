/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <assert.h>
#include <stdio.h>

#include "common/crypto/curl-p/hamming.h"
#include "common/crypto/curl-p/ptrit.h"
#include "common/crypto/curl-p/search.h"
#include "common/crypto/curl-p/trit.h"

static test_result_t test(pcurl_t const *pcurl, test_arg_t security) {
  size_t i;
  assert(security <= 3);

  for (i = 0; i < PTRIT_SIZE; i++) {
    ptrit_t const *p = pcurl->state;
    ssize_t sum = 0;
    size_t j = 0;

    for (; j++ < (size_t)security;) {
      sum += ptrits_sum_slice(HASH_LENGTH_TRYTE, p, i);
      p += HASH_LENGTH_TRYTE;

      if (0 == sum) {
        break;
      }
    }

    if ((size_t)security == j) {
      break;
    }
  }

  return (PTRIT_SIZE == i) ? (test_result_t)-1 : (test_result_t)i;
}

PearlDiverStatus hamming(Curl const *ctx, size_t begin, size_t end, intptr_t security) {
  return pd_search(ctx, begin, end, test, security);
}
