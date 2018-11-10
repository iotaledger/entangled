/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __ISS_CURL_H_
#define __ISS_CURL_H_

#include "common/curl-p/trit.h"

#define HASH_PREFIX curl
#define HASH_STATE Curl

#ifdef __cplusplus
extern "C" {
#endif

#include "iss.h.inc"

#ifdef __cplusplus
}
#endif

#undef HASH_PREFIX
#undef HASH_STATE

#endif /* __ISS_CURL_H_ */
