/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __COMMON_NETWORK_HASH_REQUEST_H__
#define __COMMON_NETWORK_HASH_REQUEST_H__

#include "common/network/neighbor.h"
#include "common/trinary/trit_array.h"

typedef struct {
  neighbor_t *neighbor;
  trit_array_p hash;
} hash_request_t;

#endif  // __COMMON_NETWORK_HASH_REQUEST_H__
