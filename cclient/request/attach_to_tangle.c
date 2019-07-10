/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/request/attach_to_tangle.h"

attach_to_tangle_req_t *attach_to_tangle_req_new() {
  attach_to_tangle_req_t *req = (attach_to_tangle_req_t *)malloc(sizeof(attach_to_tangle_req_t));
  if (req) {
    req->mwm = ATTACH_TO_TANGLE_MAIN_MWM;
    req->trytes = hash8019_array_new();
    memset(req->trunk, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    memset(req->branch, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }
  return req;
}

void attach_to_tangle_req_free(attach_to_tangle_req_t **req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->trytes) {
    hash_array_free((*req)->trytes);
  }

  free(*req);
  *req = NULL;
}

void attach_to_tangle_req_init(attach_to_tangle_req_t *req, flex_trit_t const *const trunk,
                               flex_trit_t const *const branch, uint8_t mwm) {
  memcpy(req->trunk, trunk, FLEX_TRIT_SIZE_243);
  memcpy(req->branch, branch, FLEX_TRIT_SIZE_243);
  req->mwm = mwm;
}

retcode_t attach_to_tangle_req_trytes_add(attach_to_tangle_req_t *req, flex_trit_t const *const raw_trytes) {
  if (!req->trytes) {
    req->trytes = hash8019_array_new();
  }
  if (!req->trytes) {
    return RC_OOM;
  }
  hash_array_push(req->trytes, raw_trytes);
  return RC_OK;
}

flex_trit_t *attach_to_tangle_req_trytes_get(attach_to_tangle_req_t *req, size_t index) {
  if (!req->trytes) {
    return NULL;
  }
  return hash_array_at(req->trytes, index);
}
