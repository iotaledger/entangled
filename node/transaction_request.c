/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "node/transaction_request.h"

size_t transaction_request_queue_count(transaction_request_queue_t const queue) {
  transaction_request_queue_entry_t *iter = NULL;
  size_t count = 0;

  CDL_COUNT(queue, iter, count);

  return count;
}

retcode_t transaction_request_queue_push(transaction_request_queue_t *const queue, neighbor_t *const neighbor,
                                         flex_trit_t const *const hash) {
  transaction_request_queue_entry_t *entry = NULL;

  if (queue == NULL) {
    return RC_NULL_PARAM;
  }

  if ((entry = (transaction_request_queue_entry_t *)malloc(sizeof(transaction_request_queue_entry_t))) == NULL) {
    return RC_OOM;
  }
  entry->request.neighbor = neighbor;
  memcpy(entry->request.hash, hash, FLEX_TRIT_SIZE_243);
  CDL_APPEND(*queue, entry);

  return RC_OK;
}

transaction_request_queue_entry_t *transaction_request_queue_pop(transaction_request_queue_t *const queue) {
  transaction_request_queue_entry_t *front = NULL;

  if (queue == NULL) {
    return NULL;
  }

  front = *queue;
  if (front != NULL) {
    CDL_DELETE(*queue, front);
  }

  return front;
}

void transaction_request_queue_free(transaction_request_queue_t *const queue) {
  transaction_request_queue_entry_t *iter = NULL, *tmp1 = NULL, *tmp2 = NULL;

  if (queue == NULL) {
    return;
  }

  CDL_FOREACH_SAFE(*queue, iter, tmp1, tmp2) {
    CDL_DELETE(*queue, iter);
    free(iter);
  }
  *queue = NULL;
}
