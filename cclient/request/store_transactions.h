/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_REQUEST_STORE_TRANSACTIONS_H
#define CCLIENT_REQUEST_STORE_TRANSACTIONS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct store_transactions_req_s {
  // List of raw transactions data to be stored
  hash8019_array_p trytes;
} store_transactions_req_t;

store_transactions_req_t* store_transactions_req_new();
void store_transactions_req_free(store_transactions_req_t** const req);
retcode_t store_transactions_req_trytes_add(store_transactions_req_t* req, flex_trit_t const* const raw_trytes);
flex_trit_t* store_transactions_req_trytes_get(store_transactions_req_t* req, size_t index);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_REQUEST_STORE_TRANSACTIONS_H
