/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_HASHCASH_H_
#define __COMMON_CURL_P_HASHCASH_H_

#include "common/crypto/curl-p/pearl_diver.h"
#include "common/crypto/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

PearlDiverStatus hashcash(Curl const *ctx, size_t begin, size_t end, intptr_t min_weight);

#ifdef __cplusplus
}
#endif

#endif
