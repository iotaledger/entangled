/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/get_trytes.h"

get_trytes_res_t* get_trytes_res_new() { return flex_hash_array_new(); }

trit_array_p get_trytes_res_at(get_trytes_res_t* trytes_array, int index) {
  return flex_hash_array_at(trytes_array, index);
}

int get_trytes_res_num(get_trytes_res_t* trytes_array) {
  return flex_hash_array_count(trytes_array);
}

void get_trytes_res_free(get_trytes_res_t* trytes_array) {
  flex_hash_array_free(trytes_array);
}
