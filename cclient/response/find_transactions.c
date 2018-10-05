/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/find_transactions.h"

find_transactions_res_t* find_transactions_res_new() {
  return flex_hash_array_new();
}

trit_array_p find_transactions_res_hash_at(find_transactions_res_t* in,
                                           int index) {
  return flex_hash_array_at(in, index);
}

int find_transactions_res_hash_num(find_transactions_res_t* in) {
  return flex_hash_array_count(in);
}

void find_transactions_res_free(find_transactions_res_t* res) {
  flex_hash_array_free(res);
}
