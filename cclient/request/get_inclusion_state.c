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
    req->hashes = NULL;
    req->tips = NULL;
  }
  return req;
}

void get_inclusion_state_req_free(get_inclusion_state_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->hashes) {
    hash243_queue_free(&(*req)->hashes);
  }
  if ((*req)->tips) {
    hash243_queue_free(&(*req)->tips);
  }
  free(*req);
  *req = NULL;
}
