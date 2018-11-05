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
    req->hashes = NULL;
  }
  return req;
}

void get_trytes_req_free(get_trytes_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->hashes) {
    hash243_queue_free(&(*req)->hashes);
  }

  free(*req);
  *req = NULL;
}

retcode_t get_trytes_req_add_hash(get_trytes_req_t* const req,
                                  flex_trit_t const* const hash) {
  return hash243_queue_push(&req->hashes, hash);
}
