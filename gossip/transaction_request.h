/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __GOSSIP_TRANSACTION_REQUEST_H__
#define __GOSSIP_TRANSACTION_REQUEST_H__

// Forward declarations
typedef struct neighbor_s neighbor_t;
typedef struct _trit_array *trit_array_p;

typedef struct transaction_request_s {
  neighbor_t *neighbor;
  trit_array_p hash;
} transaction_request_t;

#endif  // __GOSSIP_TRANSACTION_REQUEST_H__
