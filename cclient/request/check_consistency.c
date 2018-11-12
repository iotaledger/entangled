/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/check_consistency.h"

check_consistency_req_t* check_consistency_req_new() {
  check_consistency_req_t* req =
      (check_consistency_req_t*)malloc(sizeof(check_consistency_req_t));
  if (req) {
    req->hashes = NULL;
  }
  return req;
}

void check_consistency_req_free(check_consistency_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->hashes) {
    hash243_queue_free(&(*req)->hashes);
  }
  free(*req);
  *req = NULL;
}
