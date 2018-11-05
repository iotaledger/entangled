/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_HASHCASH_H_
#define __COMMON_CURL_P_HASHCASH_H_

#include "common/curl-p/pearl_diver.h"
#include "common/curl-p/trit.h"

typedef enum {
  TAIL,
  BODY,
  HEAD,
} SearchType;

#ifdef __cplusplus
extern "C" {
#endif

PearlDiverStatus hashcash(Curl *const ctx, SearchType const type,
                          unsigned short const offset, unsigned short const end,
                          unsigned short const min_weight);

#ifdef __cplusplus
}
#endif

#endif
