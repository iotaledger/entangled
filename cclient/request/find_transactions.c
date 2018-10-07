/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/find_transactions.h"

find_transactions_req_t* find_transactions_req_new() {
  find_transactions_req_t* req =
      (find_transactions_req_t*)malloc(sizeof(find_transactions_req_t));
  req->addresses = flex_hash_array_new();
  req->tags = flex_hash_array_new();
  req->approvees = flex_hash_array_new();
  req->bundles = flex_hash_array_new();
  return req;
}

void find_transactions_req_free(find_transactions_req_t** req) {
  if (*req) {
    flex_hash_array_free((*req)->addresses);
    flex_hash_array_free((*req)->tags);
    flex_hash_array_free((*req)->approvees);
    flex_hash_array_free((*req)->bundles);
    free(*req);
    *req = NULL;
  }
}

find_transactions_req_t* find_transactions_req_add_address(
    find_transactions_req_t* req, char* addr) {
  req->addresses = flex_hash_array_append(req->addresses, addr);
  return req;
}
find_transactions_req_t* find_transactions_req_add_tag(
    find_transactions_req_t* req, char* tag) {
  req->tags = flex_hash_array_append(req->tags, tag);
  return req;
}
find_transactions_req_t* find_transactions_req_add_approvee(
    find_transactions_req_t* req, char* approvee) {
  req->approvees = flex_hash_array_append(req->approvees, approvee);
  return req;
}
find_transactions_req_t* find_transactions_req_add_bundle(
    find_transactions_req_t* req, char* bundle) {
  req->bundles = flex_hash_array_append(req->bundles, bundle);
  return req;
}
