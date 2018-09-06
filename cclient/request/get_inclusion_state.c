/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_inclusion_state.h"

get_inclusion_state_req_t* get_inclusion_state_req_new() {
  get_inclusion_state_req_t* req =
      (get_inclusion_state_req_t*)malloc(sizeof(get_inclusion_state_req_t));
  if (req) {
    utarray_new(req->hashes, &ut_str_icd);
    utarray_new(req->tips, &ut_str_icd);
  }
  return req;
}

void get_inclusion_state_req_free(get_inclusion_state_req_t** req) {
  if (*req) {
    utarray_free((*req)->hashes);
    utarray_free((*req)->tips);
    free(*req);
    *req = NULL;
  }
}

void get_inclusion_state_req_add_hash(get_inclusion_state_req_t* req,
                                      char* hash) {
  utarray_push_back(req->hashes, &hash);
}

void get_inclusion_state_req_add_tip(get_inclusion_state_req_t* req,
                                     char* tip) {
  utarray_push_back(req->tips, &tip);
}
