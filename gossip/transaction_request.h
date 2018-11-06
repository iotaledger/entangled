/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_TRANSACTION_REQUEST_H__
#define __GOSSIP_TRANSACTION_REQUEST_H__

#include "common/trinary/flex_trit.h"

// Forward declarations
typedef struct neighbor_s neighbor_t;

typedef struct transaction_request_s {
  neighbor_t *neighbor;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} transaction_request_t;

#endif  // __GOSSIP_TRANSACTION_REQUEST_H__
