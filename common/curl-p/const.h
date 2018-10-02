/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_CONST_H
#define __COMMON_CURL_P_CONST_H

#include "common/defs.h"

#define STATE_LENGTH 3 * HASH_LENGTH_TRIT

typedef enum {
  CURL_P_27 = 27,
  CURL_P_81 = 81,
} CurlType;

#endif
