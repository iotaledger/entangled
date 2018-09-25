/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "get_tips.h"

get_tips_res_t* get_tips_res_new() { return flex_hash_array_new(); }

trit_array_p get_tips_res_hash_at(get_tips_res_t* hashes, size_t index) {
  return flex_hash_array_at(hashes, index);
}

int get_tips_res_hash_num(get_tips_res_t* hashes) {
  return flex_hash_array_count(hashes);
}

void get_tips_res_free(get_tips_res_t* hashes) { flex_hash_array_free(hashes); }
