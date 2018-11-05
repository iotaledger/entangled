/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_SEARCH_H_
#define __COMMON_CURL_P_SEARCH_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/ptrit.h"
#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

PearlDiverStatus pd_search(Curl *const ctx, unsigned short const offset,
                           unsigned short const end,
                           short (*test)(PCurl *const, unsigned short const),
                           unsigned short const param);

#ifdef __cplusplus
}
#endif

#endif
