/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_STORE_TRANSACTIONS_H
#define CCLIENT_REQUEST_STORE_TRANSACTIONS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct store_transactions_req_s {
  // List of raw transactions data to be stored
  flex_hash_array_t* trytes;
} store_transactions_req_t;

store_transactions_req_t* store_transactions_req_new();
void store_transactions_req_free(store_transactions_req_t** const req);
store_transactions_req_t* store_transactions_req_add_trytes(
    store_transactions_req_t* const req, tryte_t const* const trytes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_STORE_TRANSACTIONS_H
