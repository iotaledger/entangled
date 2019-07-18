/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>

#include "cclient/response/get_balances.h"

static const UT_icd ut_uint64_icd = {sizeof(uint64_t), NULL, NULL, NULL};

get_balances_res_t* get_balances_res_new() {
  get_balances_res_t* res = (get_balances_res_t*)malloc(sizeof(get_balances_res_t));
  if (res) {
    utarray_new(res->balances, &ut_uint64_icd);
    res->references = NULL;
    res->milestone_index = 0;
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
  if ((*res)->references) {
    hash243_queue_free(&(*res)->references);
  }
  free(*res);
  *res = NULL;
}

size_t get_balances_res_balances_num(get_balances_res_t const* const res) {
  if (!res) {
    return 0;
  }
  return utarray_len(res->balances);
}

uint64_t get_balances_res_balances_at(get_balances_res_t const* const res, size_t const index) {
  if (!res) {
    return 0;
  }
  return *(uint64_t*)utarray_eltptr(res->balances, index);
}

retcode_t get_balances_res_balances_add(get_balances_res_t* const res, uint64_t value) {
  if (!res) {
    return RC_NULL_PARAM;
  }

  if (!res->balances) {
    utarray_new(res->balances, &ut_uint64_icd);
  }

  if (!res->balances) {
    return RC_OOM;
  }
  utarray_push_back(res->balances, &value);
  return RC_OK;
}
