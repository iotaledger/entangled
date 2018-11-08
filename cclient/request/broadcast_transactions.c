/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "request/broadcast_transactions.h"

broadcast_transactions_req_t* broadcast_transactions_req_new() {
  broadcast_transactions_req_t* req = (broadcast_transactions_req_t*)malloc(
      sizeof(broadcast_transactions_req_t));
  if (req) {
    req->trytes = NULL;
  }
  return req;
}

void broadcast_transactions_req_free(broadcast_transactions_req_t** const req) {
  if (!req || !(*req)) {
    return;
  }
  broadcast_transactions_req_t* tmp = *req;
  if (tmp->trytes) {
    hash8019_queue_free(&tmp->trytes);
  }
  free(tmp);
  *req = NULL;
}

retcode_t broadcast_transactions_req_add_trytes(
    broadcast_transactions_req_t* const req, tryte_t const* const trytes) {
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                                      trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                                      NUM_TRYTES_SERIALIZED_TRANSACTION);
  if (len) {
    hash8019_queue_push(&req->trytes, trits);
  }
  return len ? RC_OK : RC_CCLIENT_FLEX_TRITS;
}
