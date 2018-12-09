/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_PTRIT_H_
#define __COMMON_CURL_P_PTRIT_H_

#include "common/curl-p/const.h"
#include "common/trinary/ptrit_incr.h"

typedef struct {
  ptrit_t state[STATE_LENGTH];
  CurlType type;
} PCurl;

#ifdef __cplusplus
extern "C" {
#endif

void init_ptrit_curl(PCurl* const ctx);
void ptrit_transform(PCurl* const ctx);
void ptrit_curl_absorb(PCurl* const ctx, ptrit_t const* const trits,
                       size_t const length);
void ptrit_curl_squeeze(PCurl* const ctx, ptrit_t* const trits,
                        size_t const length);
void ptrit_curl_reset(PCurl* const ctx);

#ifdef __cplusplus
}
#endif

#endif
