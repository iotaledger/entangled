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
    memset(res->trunk, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    memset(res->branch, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  }
  return res;
}

void get_transactions_to_approve_res_free(
    get_transactions_to_approve_res_t** res) {
  if (res || (*res)) {
    free(*res);
    *res = NULL;
  }
}
