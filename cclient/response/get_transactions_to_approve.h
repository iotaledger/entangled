/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
#define CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_transactions_to_approve_res_s {
  flex_trit_t branch[FLEX_TRIT_SIZE_243];
  flex_trit_t trunk[FLEX_TRIT_SIZE_243];
} get_transactions_to_approve_res_t;

get_transactions_to_approve_res_t* get_transactions_to_approve_res_new();
void get_transactions_to_approve_res_free(get_transactions_to_approve_res_t** const res);
static inline void get_transactions_to_approve_res_set_branch(get_transactions_to_approve_res_t* const res,
                                                              flex_trit_t const* const branch) {
  memcpy(res->branch, branch, FLEX_TRIT_SIZE_243);
}
static inline flex_trit_t* const get_transactions_to_approve_res_branch(get_transactions_to_approve_res_t* const res) {
  return res->branch;
}

static inline void get_transactions_to_approve_res_set_trunk(get_transactions_to_approve_res_t* const res,
                                                             flex_trit_t const* const trunk) {
  memcpy(res->trunk, trunk, FLEX_TRIT_SIZE_243);
}
static inline flex_trit_t* const get_transactions_to_approve_res_trunk(get_transactions_to_approve_res_t* const res) {
  return res->trunk;
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TRANSACTIONS_TO_APPROVE_H
