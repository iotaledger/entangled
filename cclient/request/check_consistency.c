/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/check_consistency.h"

check_consistency_req_t* check_consistency_req_new() {
  check_consistency_req_t* req =
      (check_consistency_req_t*)malloc(sizeof(check_consistency_req_t));
  if (req) {
    req->hashes = NULL;
  }
  return req;
}
retcode_t check_consistency_req_add(check_consistency_req_t* req,
                                    tryte_t const* const tail) {
  flex_trit_t trits[FLEX_TRIT_SIZE_243];
  size_t len = flex_trits_from_trytes(trits, NUM_TRITS_HASH, tail,
                                      NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  if (len) {
    hash243_queue_push(&req->hashes, trits);
  }
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}
void check_consistency_req_free(check_consistency_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->hashes) {
    hash243_queue_free(&(*req)->hashes);
  }
  free(*req);
  *req = NULL;
}
