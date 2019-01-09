/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_TRANSACTION_REQUEST_H__
#define __GOSSIP_TRANSACTION_REQUEST_H__

#include "common/errors.h"
#include "common/trinary/flex_trit.h"

// Forward declarations
typedef struct neighbor_s neighbor_t;

// A transaction request coming from a neighbor
typedef struct transaction_request_s {
  // The requesting neighbor
  neighbor_t* neighbor;
  // The requested hash
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
} transaction_request_t;

#endif  // __GOSSIP_TRANSACTION_REQUEST_H__
