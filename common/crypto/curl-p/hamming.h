/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_HAMMING_H_
#define __COMMON_CURL_P_HAMMING_H_

#include "common/crypto/curl-p/pearl_diver.h"
#include "common/crypto/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

PearlDiverStatus hamming(Curl const *ctx, size_t begin, size_t end, intptr_t security);

#ifdef __cplusplus
}
#endif

#endif
