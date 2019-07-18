/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/response/get_missing_transactions.h"

get_missing_transactions_res_t* get_missing_transactions_res_new() {
  get_missing_transactions_res_t* res = (get_missing_transactions_res_t*)malloc(sizeof(get_missing_transactions_res_t));
  if (res) {
    res->hashes = NULL;
  }
  return res;
}

size_t get_missing_transactions_res_hash_num(get_missing_transactions_res_t* res) {
  if (!res) {
    return 0;
  }
  return hash243_stack_count(res->hashes);
}

void get_missing_transactions_res_free(get_missing_transactions_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->hashes) {
    hash243_stack_free(&(*res)->hashes);
  }
  free(*res);
  *res = NULL;
}
