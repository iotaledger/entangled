/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_PTRIT_H_
#define __COMMON_CURL_P_PTRIT_H_

#include "common/crypto/curl-p/const.h"
#include "common/trinary/ptrit.h"
#include "utils/memset_safe.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  ptrit_t state[STATE_LENGTH];
  CurlType type;
} PCurl;

void ptrit_curl_init(PCurl* const ctx, CurlType type);
void ptrit_curl_absorb(PCurl* const ctx, ptrit_t const* const trits, size_t length);
void ptrit_curl_squeeze(PCurl* const ctx, ptrit_t* const trits, size_t length);
void ptrit_transform(PCurl* const ctx);
void ptrit_curl_reset(PCurl* const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CURL_P_PTRIT_H_
