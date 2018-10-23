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
    res->hashes = flex_hash_array_new();
  }
  return res;
}

trit_array_p find_transactions_res_hash_at(find_transactions_res_t* in,
                                           int index) {
  return flex_hash_array_at(in->hashes, index);
}

size_t find_transactions_res_hash_num(find_transactions_res_t* in) {
  return flex_hash_array_count(in->hashes);
}

void find_transactions_res_free(find_transactions_res_t** res) {
  if (!res || !(*res)) {
    return;
  }

  if ((*res)->hashes) {
    flex_hash_array_free((*res)->hashes);
  }
  free(*res);
  *res = NULL;
}
