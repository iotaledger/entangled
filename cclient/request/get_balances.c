/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/request/get_balances.h"

get_balances_req_t* get_balances_req_new() {
  get_balances_req_t* req = (get_balances_req_t*)malloc(sizeof(get_balances_req_t));
  if (req) {
    req->addresses = NULL;
    req->tips = NULL;
    req->threshold = 0;
  }
  return req;
}

void get_balances_req_free(get_balances_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->addresses) {
    hash243_queue_free(&(*req)->addresses);
  }
  if ((*req)->tips) {
    hash243_queue_free(&(*req)->tips);
  }

  free(*req);
  *req = NULL;
}
