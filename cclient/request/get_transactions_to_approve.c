/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/request/get_transactions_to_approve.h"

get_transactions_to_approve_req_t* get_transactions_to_approve_req_new() {
  get_transactions_to_approve_req_t* req =
      (get_transactions_to_approve_req_t*)calloc(1, sizeof(get_transactions_to_approve_req_t));
  return req;
}

void get_transactions_to_approve_req_free(get_transactions_to_approve_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }

  get_transactions_to_approve_req_t* tmp = *req;

  if (tmp->reference) {
    free(tmp->reference);
  }
  free(tmp);
  *req = NULL;
}

void get_transactions_to_approve_req_set_depth(get_transactions_to_approve_req_t* const req, uint32_t const depth) {
  if (req) {
    req->depth = depth;
  }
}

retcode_t get_transactions_to_approve_req_set_reference(get_transactions_to_approve_req_t* const req,
                                                        flex_trit_t const* const reference) {
  if (!req || !reference) {
    return RC_NULL_PARAM;
  }

  if (req->reference == NULL) {
    req->reference = (flex_trit_t*)malloc(FLEX_TRIT_SIZE_243);
  }

  if (req->reference == NULL) {
    return RC_OOM;
  }

  memcpy(req->reference, reference, FLEX_TRIT_SIZE_243);
  return RC_OK;
}
