/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_CURL_P_CONST_H
#define __COMMON_CURL_P_CONST_H

#include "common/defs.h"
#include "common/trinary/trits.h"

#define __TRUTH_TABLE 1, 0, -1, 2, 1, -1, 0, 2, -1, 1, 0
#define STATE_LENGTH 3 * HASH_LENGTH_TRIT

extern const size_t CURL_INDEX[STATE_LENGTH + 1];
extern const trit_t TRUTH_TABLE[11];

typedef enum {
  CURL_P_27 = 27,
  CURL_P_81 = 81,
} CurlType;

#endif
