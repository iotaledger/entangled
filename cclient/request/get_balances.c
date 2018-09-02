/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_balances.h"

get_balances_req_t* get_balances_req_new() {
  get_balances_req_t* req =
      (get_balances_req_t*)malloc(sizeof(get_balances_req_t));
  utarray_new(req->addresses, &ut_str_icd);
  utarray_new(req->tips, &ut_str_icd);

  return req;
}

void get_balances_req_free(get_balances_req_t* req) {
  if (req) {
    utarray_free(req->addresses);
    utarray_free(req->tips);
    free(req);
  }
}

void get_balances_req_add_address(get_balances_req_t* req, char* addr) {
  utarray_push_back(req->addresses, &addr);
}

void get_balances_req_add_tips(get_balances_req_t* req, char* tips) {
  utarray_push_back(req->tips, &tips);
}
