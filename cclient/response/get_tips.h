/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TIPS_H
#define CCLIENT_RESPONSE_GET_TIPS_H

#include "cclient/types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_tips_res_s {
  hash243_stack_t hashes;
} get_tips_res_t;

get_tips_res_t* get_tips_res_new();
size_t get_tips_res_hash_num(get_tips_res_t* res);
void get_tips_res_free(get_tips_res_t** res);
static inline retcode_t get_tips_res_hashes_push(get_tips_res_t* res, flex_trit_t const* const hash) {
  return hash243_stack_push(&res->hashes, hash);
}
static inline void get_tips_res_hashes_pop(get_tips_res_t* res, flex_trit_t* const buf) {
  memcpy(buf, hash243_stack_peek(res->hashes), FLEX_TRIT_SIZE_243);
  hash243_stack_pop(&res->hashes);
}

static inline retcode_t get_tips_res_hashes_add(get_tips_res_t* const res, flex_trit_t const* const hash) {
  return hash243_stack_push(&res->hashes, hash);
}

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H
