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
    utarray_new(res->balances, &ut_str_icd);
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

char* get_balances_res_balances_at(get_balances_res_t const* const in,
                                   int const index) {
  char** p = (char**)utarray_eltptr(in->balances, index);
  return *p;
}

size_t get_balances_res_total_balance(get_balances_res_t const* const res) {
  size_t sum = 0;
  size_t elm_count = utarray_len(res->balances);
  for (int i = 0; i < elm_count; i++) {
    char** p = (char**)utarray_eltptr(res->balances, i);
    char* endptr;
    sum += strtol(*p, &endptr, 10);
  }
  return sum;
}
