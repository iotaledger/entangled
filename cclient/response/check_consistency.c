/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "response/check_consistency.h"

check_consistency_res_t* check_consistency_res_new() {
  check_consistency_res_t* res =
      (check_consistency_res_t*)malloc(sizeof(check_consistency_res_t));

  res->info = char_buffer_new();
  return res;
}

void check_consistency_res_free(check_consistency_res_t* res) {
  if (res) {
    char_buffer_free(res->info);
    free(res);
  }
}
