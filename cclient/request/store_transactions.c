/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "store_transactions.h"

store_transactions_req_t* store_transactions_req_new() {
  return flex_hash_array_new();
}

store_transactions_req_t* store_transactions_req_add(
    store_transactions_req_t* transactions, const char* trytes) {
  return flex_hash_array_append(transactions, trytes);
}

void store_transactions_req_free(store_transactions_req_t* transactions) {
  flex_hash_array_free(transactions);
}
