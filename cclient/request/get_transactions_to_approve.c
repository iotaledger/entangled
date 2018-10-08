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
  if (req == NULL) {
    return NULL;
  }

  req->reference = char_buffer_new();
  return req;
}

void get_transactions_to_approve_req_free(
    get_transactions_to_approve_req_t** req) {
  if (*req) {
    char_buffer_free((*req)->reference);
    free(*req);
    *req = NULL;
  }
}

void get_transactions_to_approve_req_set_reference(
    get_transactions_to_approve_req_t* req, char* reference) {
  retcode_t ret = RC_OK;
  size_t len = strlen(reference);

  ret = char_buffer_allocate(req->reference, len);
  if (ret == RC_OK) {
    strncpy(req->reference->data, reference, len);
  }
}
