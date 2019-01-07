/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_PTRIT_H_
#define __COMMON_CURL_P_PTRIT_H_

#include "common/curl-p/const.h"
#include "common/curl-p/indices.h"
#include "common/trinary/ptrit.h"
#include "utils/forced_inline.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  ptrit_t state[STATE_LENGTH];
  CurlType type;
} PCurl;

static const size_t CURL_INDEX[STATE_LENGTH + 1] = {__INDEX_TABLE};

static FORCED_INLINE void ptrit_sbox(ptrit_t* const c, ptrit_t const* const s) {
  ptrit_s alpha, beta, gamma, delta;
  size_t i = 0;

  for (; i < STATE_LENGTH; ++i) {
    alpha = s[CURL_INDEX[i]].low;
    beta = s[CURL_INDEX[i]].high;
    gamma = s[CURL_INDEX[i + 1]].high;
    delta = (alpha | (~gamma)) & (s[CURL_INDEX[i + 1]].low ^ beta);

    c[i].low = ~delta;
    c[i].high = (alpha ^ gamma) | delta;
  }
}

void ptrit_curl_init(PCurl* const ctx, CurlType type);
void ptrit_curl_absorb(PCurl* const ctx, ptrit_t const* const trits,
                       size_t length);
void ptrit_curl_squeeze(PCurl* const ctx, ptrit_t* const trits, size_t length);
void ptrit_curl_reset(PCurl* const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CURL_P_PTRIT_H_
