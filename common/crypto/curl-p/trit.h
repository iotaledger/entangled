/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CRYPTO_CURL_P_TRIT_H__
#define __COMMON_CRYPTO_CURL_P_TRIT_H__

#include <string.h>

#include "common/crypto/curl-p/const.h"
#include "common/trinary/trits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  trit_t state[STATE_LENGTH];
  CurlType type;
} Curl;

void curl_init(Curl* const ctx);
void curl_absorb(Curl* const ctx, trit_t const* const trits, size_t length);
void curl_squeeze(Curl* const ctx, trit_t* const trits, size_t length);
void curl_reset(Curl* const ctx);

#ifdef __cplusplus
}
#endif

#endif  // __COMMON_CRYPTO_CURL_P_TRIT_H__
