/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "find_transactions.h"

find_transactions_req_t* find_transactions_req_new() {
  find_transactions_req_t* req =
      (find_transactions_req_t*)malloc(sizeof(find_transactions_req_t));
  utarray_new(req->addresses, &ut_str_icd);
  utarray_new(req->tags, &ut_str_icd);
  utarray_new(req->approvees, &ut_str_icd);
  utarray_new(req->bundles, &ut_str_icd);
  return req;
}

void find_transactions_req_free(find_transactions_req_t** req) {
  if (*req) {
    utarray_free((*req)->addresses);
    utarray_free((*req)->tags);
    utarray_free((*req)->approvees);
    utarray_free((*req)->bundles);
    free(*req);
    *req = NULL;
  }
}

void find_transactions_req_add_address(find_transactions_req_t* req,
                                       char* addr) {
  utarray_push_back(req->addresses, &addr);
}
void find_transactions_req_add_tag(find_transactions_req_t* req, char* tag) {
  utarray_push_back(req->tags, &tag);
}
void find_transactions_req_add_approvee(find_transactions_req_t* req,
                                        char* approvee) {
  utarray_push_back(req->approvees, &approvee);
}
void find_transactions_req_add_bundle(find_transactions_req_t* req,
                                      char* bundle) {
  utarray_push_back(req->bundles, &bundle);
}
