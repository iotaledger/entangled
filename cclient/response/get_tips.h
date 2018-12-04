/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TIPS_H
#define CCLIENT_RESPONSE_GET_TIPS_H

#include "types/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct get_tips_res_s {
  hash243_queue_t hashes;
} get_tips_res_t;

get_tips_res_t* get_tips_res_new();
flex_trit_t* get_tips_res_hash_at(get_tips_res_t* res, size_t index);
size_t get_tips_res_hash_num(get_tips_res_t* res);
void get_tips_res_free(get_tips_res_t** res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H
