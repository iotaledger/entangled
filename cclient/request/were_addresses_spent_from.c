/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/request/were_addresses_spent_from.h"

were_addresses_spent_from_req_t* were_addresses_spent_from_req_new() {
  were_addresses_spent_from_req_t* req =
      (were_addresses_spent_from_req_t*)malloc(sizeof(were_addresses_spent_from_req_t));
  if (req) {
    req->addresses = NULL;
  }
  return req;
}

void were_addresses_spent_from_req_free(were_addresses_spent_from_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->addresses) {
    hash243_queue_free(&(*req)->addresses);
  }

  free(*req);
  *req = NULL;
}
