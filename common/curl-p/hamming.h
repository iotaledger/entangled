/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_HAMMING_H_
#define __COMMON_CURL_P_HAMMING_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

PearlDiverStatus hamming(Curl *const, unsigned short const offset,
                         unsigned short const end,
                         unsigned short const security);

#ifdef __cplusplus
}
#endif

#endif
