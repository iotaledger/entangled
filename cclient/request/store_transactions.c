/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/store_transactions.h"

store_transactions_req_t* store_transactions_req_new() {
  store_transactions_req_t* req =
      (store_transactions_req_t*)malloc(sizeof(store_transactions_req_t));
  if (req) {
    req->trytes = NULL;
  }
  return req;
}

void store_transactions_req_free(store_transactions_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  store_transactions_req_t* tmp = *req;

  if (tmp->trytes) {
    hash8019_stack_free(&tmp->trytes);
  }
  free(tmp);
  *req = NULL;
}
