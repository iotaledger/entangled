/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_transactions_to_approve.h"

get_transactions_to_approve_res_t* get_transactions_to_approve_res_new() {
  get_transactions_to_approve_res_t* res =
      (get_transactions_to_approve_res_t*)malloc(
          sizeof(get_transactions_to_approve_res_t));
  if (res) {
    res->trunk = trit_array_new(NUM_TRITS_ADDRESS);
    res->branch = trit_array_new(NUM_TRITS_ADDRESS);
  }
  return res;
}

void get_transactions_to_approve_res_free(
    get_transactions_to_approve_res_t** res) {
  if (*res) {
    trit_array_free((*res)->trunk);
    trit_array_free((*res)->branch);
    free(*res);
    *res = NULL;
  }
}
