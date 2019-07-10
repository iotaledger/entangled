/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/broadcast_transactions.h"

broadcast_transactions_req_t *broadcast_transactions_req_new() {
  broadcast_transactions_req_t *req = (broadcast_transactions_req_t *)malloc(sizeof(broadcast_transactions_req_t));
  if (req) {
    req->trytes = NULL;
  }

  return req;
}

void broadcast_transactions_req_free(broadcast_transactions_req_t **const req) {
  if (!req || !(*req)) {
    return;
  }

  broadcast_transactions_req_t *tmp = *req;

  if (tmp->trytes) {
    hash_array_free(tmp->trytes);
  }

  free(tmp);
  *req = NULL;
}

retcode_t broadcast_transactions_req_trytes_add(broadcast_transactions_req_t *req,
                                                flex_trit_t const *const raw_trytes) {
  if (!req->trytes) {
    req->trytes = hash8019_array_new();
  }

  if (!req->trytes) {
    return RC_OOM;
  }

  hash_array_push(req->trytes, raw_trytes);

  return RC_OK;
}

flex_trit_t *broadcat_transactions_req_trytes_get(broadcast_transactions_req_t *req, size_t index) {
  if (!req->trytes) {
    return NULL;
  }
  return hash_array_at(req->trytes, index);
}
