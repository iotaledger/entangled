/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_RESPONSE_GET_TIPS_H
#define CCLIENT_RESPONSE_GET_TIPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types/types.h"

typedef UT_array get_tips_res_t;

get_tips_res_t* get_tips_res_new();
void get_tips_res_add(get_tips_res_t* hashes, const char* hash);
char* get_tips_res_hash_at(get_tips_res_t* hashes, int index);
int get_tips_res_hash_num(get_tips_res_t* hashes);
void get_tips_res_free(get_tips_res_t* ut);
#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_RESPONSE_GET_TIPS_H
