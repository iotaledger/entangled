/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_trytes.h"

get_trytes_req_t* get_trytes_req_new() {
  get_trytes_req_t* req = (get_trytes_req_t*)malloc(sizeof(get_trytes_req_t));
  if (req) {
    req->hashes = flex_hash_array_new();
  }
  return req;
}

void get_trytes_req_free(get_trytes_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  get_trytes_req_t* tmp = *req;

  if (tmp->hashes) {
    flex_hash_array_free(tmp->hashes);
  }
  free(tmp);
  *req = NULL;
}

get_trytes_req_t* get_trytes_req_add_hash(get_trytes_req_t* const req,
                                          tryte_t const* const hash) {
  req->hashes = flex_hash_array_append(req->hashes, (char*)hash);
  return req;
}
