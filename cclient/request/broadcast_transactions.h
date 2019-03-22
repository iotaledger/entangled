/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
#define CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct broadcast_transactions_req_s {
  // List of raw transactions data to be broadcasted
  hash8019_array_p trytes;
} broadcast_transactions_req_t;

broadcast_transactions_req_t* broadcast_transactions_req_new();
void broadcast_transactions_req_free(broadcast_transactions_req_t** const req);
// add trytes to array
retcode_t broadcast_transactions_req_trytes_add(broadcast_transactions_req_t* req, flex_trit_t const* const raw_trytes);
// get trytes from array.
flex_trit_t* broadcat_transactions_req_trytes_get(broadcast_transactions_req_t* req, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_BROADCAST_TRANSACTIONS_H
