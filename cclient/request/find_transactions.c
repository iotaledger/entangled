/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/request/find_transactions.h"

find_transactions_req_t* find_transactions_req_new() {
  find_transactions_req_t* req = (find_transactions_req_t*)malloc(sizeof(find_transactions_req_t));
  if (req) {
    req->bundles = NULL;
    req->addresses = NULL;
    req->tags = NULL;
    req->approvees = NULL;
  }
  return req;
}

void find_transactions_req_free(find_transactions_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->bundles) {
    hash243_queue_free(&(*req)->bundles);
  }
  if ((*req)->addresses) {
    hash243_queue_free(&(*req)->addresses);
  }
  if ((*req)->tags) {
    hash81_queue_free(&(*req)->tags);
  }
  if ((*req)->approvees) {
    hash243_queue_free(&(*req)->approvees);
  }
  free(*req);
  *req = NULL;
}
