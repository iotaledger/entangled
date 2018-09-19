/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "attach_to_tangle.h"

attach_to_tangle_res_t* attach_to_tangle_res_new() {
  return flex_hash_array_new();
}
char_buffer_t* attach_to_tangle_res_trytes_at(attach_to_tangle_res_t* res,
                                              int index) {
  // need to free `out_trytes` outside this function.
  retcode_t ret = RC_OK;
  char_buffer_t* out_trytes = char_buffer_new();
  if (out_trytes) {
    flex_hash_t* hash = flex_hash_array_at((flex_hash_array_t*)res, index);
    if (hash) {
      ret = char_buffer_allocate(out_trytes, hash->len_trits);
      if (ret != RC_OK) {
        return NULL;
      }

      ret = flex_hash_to_trytes(hash, out_trytes->data);
      if (ret == RC_OK) {
        return out_trytes;
      }
    }
  }

  return NULL;
}

int attach_to_tangle_res_trytes_cnt(attach_to_tangle_res_t* res) {
  return flex_hash_array_count((flex_hash_array_t*)res);
}

void attach_to_tangle_res_free(attach_to_tangle_res_t* hashes) {
  flex_hash_array_free(hashes);
}
