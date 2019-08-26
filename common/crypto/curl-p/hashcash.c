/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "common/crypto/curl-p/hashcash.h"
#include "common/crypto/curl-p/ptrit.h"
#include "common/crypto/curl-p/search.h"

static test_result_t test(pcurl_t const *pcurl, test_arg_t mwm) {
  ptrit_t const *p = pcurl->state + HASH_LENGTH_TRIT - mwm;
  ssize_t i = ptrits_find_zero_slice(mwm, p);
  return (PTRIT_SIZE == i) ? (test_result_t)-1 : (test_result_t)i;
}

PearlDiverStatus hashcash(Curl const *ctx, size_t begin, size_t end, intptr_t min_weight) {
  return pd_search(ctx, begin, end, &test, min_weight);
}
