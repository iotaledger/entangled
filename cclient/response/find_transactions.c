/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "find_transactions.h"

find_transactions_res_t* find_transactions_res_new() {
  find_transactions_res_t* res =
      (find_transactions_res_t*)malloc(sizeof(find_transactions_res_t));
  utarray_new(res->hashes, &ut_str_icd);
  return res;
}

char* find_transactions_res_hash_at(find_transactions_res_t* in, int index) {
  if (utarray_len(in->hashes) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(in->hashes, index);
    if (p) {
      return *p;
    }
  }
  return '\0';
}

int find_transactions_res_hash_num(find_transactions_res_t* in) {
  return utarray_len(in->hashes);
}

void find_transactions_res_free(find_transactions_res_t* res) {
  if (res) {
    utarray_free(res->hashes);
    free(res);
    res = NULL;
  }
}
