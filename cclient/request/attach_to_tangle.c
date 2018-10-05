/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/attach_to_tangle.h"

attach_to_tangle_req_t* attach_to_tangle_req_new() {
  attach_to_tangle_req_t* req =
      (attach_to_tangle_req_t*)malloc(sizeof(attach_to_tangle_req_t));
  if (req) {
    req->mwm = ATTACH_TO_TANGLE_MAIN_MWM;
    req->trytes = flex_hash_array_new();
    req->trunk = trit_array_new(NUM_TRITS_TRUNK);
    req->branch = trit_array_new(NUM_TRITS_BRANCH);
  }
  return req;
}

void attach_to_tangle_req_free(attach_to_tangle_req_t** req) {
  if (*req) {
    trit_array_free((*req)->trunk);
    trit_array_free((*req)->branch);
    flex_hash_array_free((*req)->trytes);
    free(*req);
    *req = NULL;
  }
}

retcode_t attach_to_tangle_req_set_trunk(attach_to_tangle_req_t* req,
                                         const char* trunk) {
  return trytes_to_flex_hash(req->trunk, trunk);
}

retcode_t attach_to_tangle_req_set_branch(attach_to_tangle_req_t* req,
                                          const char* branch) {
  return trytes_to_flex_hash(req->branch, branch);
}

void attach_to_tangle_req_set_mwm(attach_to_tangle_req_t* req,
                                  const int32_t mwm) {
  req->mwm = mwm;
}

flex_hash_array_t* attach_to_tangle_req_add_trytes(flex_hash_array_t* hash,
                                                   const char* trytes) {
  return flex_hash_array_append(hash, trytes);
}
