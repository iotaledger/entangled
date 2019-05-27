/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/request/check_consistency.h"

check_consistency_req_t *check_consistency_req_new() {
  check_consistency_req_t *req = (check_consistency_req_t *)malloc(sizeof(check_consistency_req_t));
  if (req) {
    req->tails = NULL;
  }
  return req;
}

void check_consistency_req_free(check_consistency_req_t **req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->tails) {
    hash243_queue_free(&(*req)->tails);
  }
  free(*req);
  *req = NULL;
}
