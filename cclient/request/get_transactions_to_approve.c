/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_transactions_to_approve.h"

get_transactions_to_approve_req_t* get_transactions_to_approve_req_new() {
  get_transactions_to_approve_req_t* req =
      (get_transactions_to_approve_req_t*)malloc(
          sizeof(get_transactions_to_approve_req_t));
  if (req) {
    memset(req->reference, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }

  return req;
}

void get_transactions_to_approve_req_free(
    get_transactions_to_approve_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  free(*req);
  *req = NULL;
}

void get_transactions_to_approve_req_set_reference(
    get_transactions_to_approve_req_t* const req,
    tryte_t const* const reference) {
  memcpy(req->reference, reference, FLEX_TRIT_SIZE_243);
}
