/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_transactions_to_approve.h"

get_transactions_to_approve_res_t* get_transactions_to_approve_res_new() {
  get_transactions_to_approve_res_t* res =
      (get_transactions_to_approve_res_t*)malloc(
          sizeof(get_transactions_to_approve_res_t));
  if (res == NULL) {
    return NULL;
  }
  res->trunk = char_buffer_new();
  res->branch = char_buffer_new();
  return res;
}

void get_transactions_to_approve_res_free(
    get_transactions_to_approve_res_t** res) {
  if (*res) {
    char_buffer_free((*res)->trunk);
    char_buffer_free((*res)->branch);
    free(*res);
    *res = NULL;
  }
}
