/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_inclusion_state.h"

get_inclusion_state_req_t* get_inclusion_state_req_new() {
  get_inclusion_state_req_t* req =
      (get_inclusion_state_req_t*)malloc(sizeof(get_inclusion_state_req_t));
  if (req) {
    req->hashes = flex_hash_array_new();
    req->tips = flex_hash_array_new();
  }
  return req;
}

void get_inclusion_state_req_free(get_inclusion_state_req_t** req) {
  if (*req) {
    flex_hash_array_free((*req)->hashes);
    flex_hash_array_free((*req)->tips);
    free(*req);
    *req = NULL;
  }
}

get_inclusion_state_req_t* get_inclusion_state_req_add_hash(
    get_inclusion_state_req_t* req, char* hash) {
  req->hashes = flex_hash_array_append(req->hashes, hash);
  return req;
}

get_inclusion_state_req_t* get_inclusion_state_req_add_tip(
    get_inclusion_state_req_t* req, char* tip) {
  req->tips = flex_hash_array_append(req->tips, tip);
  return req;
}
