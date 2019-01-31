/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/attach_to_tangle.h"

attach_to_tangle_req_t* attach_to_tangle_req_new() {
  attach_to_tangle_req_t* req =
      (attach_to_tangle_req_t*)malloc(sizeof(attach_to_tangle_req_t));
  if (req) {
    req->mwm = ATTACH_TO_TANGLE_MAIN_MWM;
    req->trytes = hash8019_array_new();
    memset(req->trunk, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    memset(req->branch, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }
  return req;
}

void attach_to_tangle_req_free(attach_to_tangle_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->trytes) {
    hash_array_free((*req)->trytes);
  }

  free(*req);
  *req = NULL;
}
