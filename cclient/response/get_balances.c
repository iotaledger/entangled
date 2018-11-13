/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_balances.h"

get_balances_res_t* get_balances_res_new() {
  get_balances_res_t* res =
      (get_balances_res_t*)malloc(sizeof(get_balances_res_t));
  if (res) {
    utarray_new(res->balances, &ut_uint64_icd);
    res->milestone = NULL;
  }
  return res;
}

void get_balances_res_free(get_balances_res_t** res) {
  if (!res || !(*res)) {
    return;
  }
  if ((*res)->balances) {
    utarray_free((*res)->balances);
  }
  if ((*res)->milestone) {
    hash243_queue_free(&(*res)->milestone);
  }
  free(*res);
  *res = NULL;
}

size_t get_balances_res_balances_num(get_balances_res_t const* const res) {
  return utarray_len(res->balances);
}

uint64_t get_balances_res_balances_at(get_balances_res_t const* const res,
                                      int const index) {
  return *(uint64_t*)utarray_eltptr(res->balances, index);
}
