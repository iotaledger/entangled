/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __COMMON_CURL_P_TRIT_H_
#define __COMMON_CURL_P_TRIT_H_

#include "common/curl-p/const.h"
#include "common/stdint.h"
#include "common/trinary/trits.h"

typedef struct {
  trit_t state[STATE_LENGTH];
  CurlType type;
} Curl;

void init_curl(Curl* ctx);

void curl_absorb(Curl* const ctx, trit_t const* const trits, size_t length);
void curl_squeeze(Curl* const ctx, trit_t* const trits, size_t length);
void curl_reset(Curl* const ctx);

#endif
#ifdef __cplusplus
}
#endif
