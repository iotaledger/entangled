/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_BCT_H
#define __COMMON_CURL_P_BCT_H

#include "common/curl-p/const.h"
#include "common/trinary/bct.h"

#define S_STATE_LENGTH 183

typedef struct {
  bct_t state[S_STATE_LENGTH];
  CurlType type;
} BCurl;

#ifdef __cplusplus
extern "C" {
#endif

void init_s_curl(BCurl* const ctx);

void s_transform(BCurl* const ctx);
void s_curl_absorb(BCurl* const ctx, bct_t const* const trits,
                   size_t const offset, size_t const length);
void s_curl_squeeze(BCurl* const ctx, bct_t* const trits, size_t const offset,
                    size_t const length);
void s_curl_reset(BCurl* const ctx);

#ifdef __cplusplus
}
#endif

#endif /* __COMMON_CURL_P_BCT_H */
