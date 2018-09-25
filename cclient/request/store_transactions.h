/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_STORE_TRANSACTIONS_H
#define CCLIENT_REQUEST_STORE_TRANSACTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"
// List of raw data of transactions to be store.
typedef flex_hash_array_t store_transactions_req_t;

store_transactions_req_t* store_transactions_req_new();
store_transactions_req_t* store_transactions_req_add(
    store_transactions_req_t* transactions, const char* trytes);
void store_transactions_req_free(store_transactions_req_t* transactions);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_STORE_TRANSACTIONS_H
