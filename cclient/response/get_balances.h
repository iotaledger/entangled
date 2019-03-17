/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_BALANCES_H
#define CCLIENT_RESPONSE_GET_BALANCES_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

static const UT_icd ut_uint64_icd UTARRAY_UNUSED = {sizeof(uint64_t), NULL, NULL, NULL};
typedef struct {
  int milestone_index;
  UT_array* balances;
  hash243_queue_t milestone;
} get_balances_res_t;

get_balances_res_t* get_balances_res_new();
void get_balances_res_free(get_balances_res_t** res);
static inline retcode_t get_balances_res_milestone_add(get_balances_res_t* const res, flex_trit_t const* const hash) {
  return hash243_queue_push(&res->milestone, hash);
}
static inline flex_trit_t* get_balances_res_milestone_get(get_balances_res_t* const res, size_t index) {
  return hash243_queue_at(&res->milestone, index);
}
size_t get_balances_res_balances_num(get_balances_res_t const* const res);
uint64_t get_balances_res_balances_at(get_balances_res_t const* const res, int const index);
retcode_t get_balances_res_balances_add(get_balances_res_t* const res, uint64_t value);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_BALANCES_H
