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
    req->trytes = NULL;
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
    hash8019_queue_free(&(*req)->trytes);
  }

  free(*req);
  *req = NULL;
}

retcode_t attach_to_tangle_req_set_trunk(attach_to_tangle_req_t* req,
                                         tryte_t const* const trunk) {
  size_t len = flex_trits_from_trytes(req->trunk, NUM_TRITS_HASH, trunk,
                                      NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}

retcode_t attach_to_tangle_req_set_branch(attach_to_tangle_req_t* req,
                                          tryte_t const* const branch) {
  size_t len = flex_trits_from_trytes(req->branch, NUM_TRITS_HASH, branch,
                                      NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}

void attach_to_tangle_req_set_mwm(attach_to_tangle_req_t* req,
                                  const int32_t mwm) {
  req->mwm = mwm;
}

retcode_t attach_to_tangle_req_add_trytes(attach_to_tangle_req_t* req,
                                          tryte_t const* const trytes) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                                      trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                                      NUM_TRYTES_SERIALIZED_TRANSACTION);
  if (len) {
    hash8019_queue_push(&req->trytes, trits);
  }
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}
