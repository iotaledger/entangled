/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/find_transactions.h"

find_transactions_res_t* find_transactions_res_new() {
  find_transactions_res_t* res =
      (find_transactions_res_t*)malloc(sizeof(find_transactions_res_t));
  if (res) {
    res->hashes = NULL;
  }
  return res;
}

void find_transactions_res_free(find_transactions_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->hashes) {
    hash243_queue_free(&(*res)->hashes);
  }
  free(*res);
  *res = NULL;
}
