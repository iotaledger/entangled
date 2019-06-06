/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_get_tips(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  get_tips_res_t *tips_res = get_tips_res_new();
  if (tips_res) {
    hash243_stack_entry_t *q_iter = NULL;
    if (iota_client_get_tips(s, tips_res) == RC_OK) {
      CDL_FOREACH(tips_res->hashes, q_iter) {
        flex_trit_print(q_iter->hash, NUM_TRITS_HASH);
        printf("\n");
      }
      printf("Tips: %lu\n", get_tips_res_hash_num(tips_res));
    }

    get_tips_res_free(&tips_res);
  } else {
    printf("Error OOM\n");
  }
}
