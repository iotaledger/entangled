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

typedef flex_hash_array_t get_tips_res_t;

get_tips_res_t* get_tips_res_new();
trit_array_p get_tips_res_hash_at(get_tips_res_t* hashes, size_t index);
int get_tips_res_hash_num(get_tips_res_t* hashes);
void get_tips_res_free(get_tips_res_t* hashes);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H
