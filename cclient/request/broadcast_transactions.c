/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "broadcast_transactions.h"

broadcast_transactions_req_t* broadcast_transactions_req_new() {
  return (broadcast_transactions_req_t*)flex_hash_array_new();
}

broadcast_transactions_req_t* broadcast_transactions_req_add(
    broadcast_transactions_req_t* transactions, const char* trytes) {
  return flex_hash_array_append((flex_hash_array_t*)transactions, trytes);
}

void broadcast_transactions_req_free(
    broadcast_transactions_req_t* transactions) {
  flex_hash_array_free(transactions);
}
