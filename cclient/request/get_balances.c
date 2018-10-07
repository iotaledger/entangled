/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_balances.h"

get_balances_req_t* get_balances_req_new() {
  get_balances_req_t* req =
      (get_balances_req_t*)malloc(sizeof(get_balances_req_t));
  if (req) {
    req->addresses = flex_hash_array_new();
    req->tips = flex_hash_array_new();
  }
  return req;
}

void get_balances_req_free(get_balances_req_t** req) {
  if (req) {
    flex_hash_array_free((*req)->addresses);
    flex_hash_array_free((*req)->tips);
    free(*req);
    *req = NULL;
  }
}

get_balances_req_t* get_balances_req_add_address(get_balances_req_t* req,
                                                 char* addr) {
  req->addresses = flex_hash_array_append(req->addresses, addr);
  return req;
}

get_balances_req_t* get_balances_req_add_tips(get_balances_req_t* req,
                                              char* tip) {
  req->tips = flex_hash_array_append(req->tips, tip);
  return req;
}
