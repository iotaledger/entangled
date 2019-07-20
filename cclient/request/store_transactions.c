/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/store_transactions.h"

store_transactions_req_t* store_transactions_req_new() {
  store_transactions_req_t* req = (store_transactions_req_t*)malloc(sizeof(store_transactions_req_t));
  if (req) {
    req->trytes = hash8019_array_new();
  }
  return req;
}

void store_transactions_req_free(store_transactions_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  store_transactions_req_t* tmp = *req;

  if (tmp->trytes) {
    hash_array_free(tmp->trytes);
  }
  free(tmp);
  *req = NULL;
}

retcode_t store_transactions_req_trytes_add(store_transactions_req_t* req, flex_trit_t const* const raw_trytes) {
  if (!req || !raw_trytes) {
    return RC_NULL_PARAM;
  }

  if (!req->trytes) {
    req->trytes = hash8019_array_new();
  }
  if (!req->trytes) {
    return RC_OOM;
  }
  hash_array_push(req->trytes, raw_trytes);
  return RC_OK;
}

flex_trit_t* store_transactions_req_trytes_get(store_transactions_req_t* req, size_t index) {
  if (!req) {
    return NULL;
  }

  if (!req->trytes) {
    return NULL;
  }
  return hash_array_at(req->trytes, index);
}
