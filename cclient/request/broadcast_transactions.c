/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/broadcast_transactions.h"

broadcast_transactions_req_t* broadcast_transactions_req_new() {
  broadcast_transactions_req_t* req = (broadcast_transactions_req_t*)malloc(
      sizeof(broadcast_transactions_req_t));
  if (req) {
    req->trytes = flex_hash_array_new();
  }
  return req;
}

void broadcast_transactions_req_free(broadcast_transactions_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }
  broadcast_transactions_req_t* tmp = *req;
  if (tmp->trytes) {
    flex_hash_array_free(tmp->trytes);
  }
  free(tmp);
  *req = NULL;
}

broadcast_transactions_req_t* broadcast_transactions_req_add_trytes(
    broadcast_transactions_req_t* const req, tryte_t const* const trytes) {
  req->trytes = flex_hash_array_append(req->trytes, (char*)trytes);
  return req;
}
