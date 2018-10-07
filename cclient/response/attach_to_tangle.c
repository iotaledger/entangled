/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/attach_to_tangle.h"

attach_to_tangle_res_t* attach_to_tangle_res_new() {
  return flex_hash_array_new();
}
trit_array_p attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res,
                                            int index) {
  return flex_hash_array_at(res, index);
}

int attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res) {
  return flex_hash_array_count(res);
}

void attach_to_tangle_res_free(attach_to_tangle_res_t* hashes) {
  flex_hash_array_free(hashes);
}
