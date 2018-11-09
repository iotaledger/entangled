/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/get_inclusion_state.h"

get_inclusion_state_req_t* get_inclusion_state_req_new() {
  get_inclusion_state_req_t* req =
      (get_inclusion_state_req_t*)malloc(sizeof(get_inclusion_state_req_t));
  if (req) {
    req->hashes = NULL;
    req->tips = NULL;
  }
  return req;
}

void get_inclusion_state_req_free(get_inclusion_state_req_t** req) {
  if (!req || !(*req)) {
    return;
  }

  if ((*req)->hashes) {
    hash243_queue_free(&(*req)->hashes);
  }
  if ((*req)->tips) {
    hash243_queue_free(&(*req)->tips);
  }
  free(*req);
  *req = NULL;
}

retcode_t get_inclusion_state_req_add_hash(get_inclusion_state_req_t* req,
                                           tryte_t const* const hash) {
  flex_trit_t trits[FLEX_TRIT_SIZE_243];
  size_t len = flex_trits_from_trytes(trits, NUM_TRITS_HASH, hash,
                                      NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  if (len) {
    hash243_queue_push(&req->hashes, trits);
  }
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}

retcode_t get_inclusion_state_req_add_tip(get_inclusion_state_req_t* req,
                                          tryte_t const* const tip) {
  flex_trit_t trits[FLEX_TRIT_SIZE_243];
  size_t len = flex_trits_from_trytes(trits, NUM_TRITS_HASH, tip,
                                      NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  if (len) {
    hash243_queue_push(&req->tips, trits);
  }
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}
