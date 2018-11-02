/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct broadcast_transactions_req_s {
  // List of raw transactions data to be broadcasted
  flex_hash_array_t* trytes;
} broadcast_transactions_req_t;

broadcast_transactions_req_t* broadcast_transactions_req_new();
void broadcast_transactions_req_free(broadcast_transactions_req_t** const req);
broadcast_transactions_req_t* broadcast_transactions_req_add_trytes(
    broadcast_transactions_req_t* const res, tryte_t const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
