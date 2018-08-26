/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __DIGEST_H
#define __DIGEST_H

#include "common/curl-p/trit.h"

#ifdef __cplusplus
extern "C" {
#endif

void curl_digest(trit_t const* const trits, size_t const len, trit_t* const out,
                 Curl* const curl);

#ifdef __cplusplus
}
#endif

#endif  //__DIGEST_H
