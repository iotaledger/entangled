/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TAIL_HASH =
    (tryte_t *)"9GKLL9R9YFXKBQRJNNGFSONCWRVDUJWQFYGWCTAAY9LWZMHEMAWVMIYYYKZXIIOZECKXBRWPEAUEGB999";

void example_get_latest_inclusion(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  hash243_queue_t txs = NULL;
  get_inclusion_states_res_t *inclusion_res = get_inclusion_states_res_new();
  if (!inclusion_res) {
    printf("Error: OOM\n");
    return;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
    goto done;
  }

  hash243_queue_push(&txs, trits_243);
  hash243_queue_push(&txs, trits_243);
  hash243_queue_push(&txs, trits_243);

  if ((ret = iota_client_get_latest_inclusion(s, txs, inclusion_res)) == RC_OK) {
    for (size_t i = 0; i < get_inclusion_states_res_states_count(inclusion_res); i++) {
      printf("[%zu]:%s\n", i, get_inclusion_states_res_states_at(inclusion_res, i) ? "true" : "false");
    }
  } else {
    printf("Error: %s\n", error_2_string(ret));
  }

done:
  get_inclusion_states_res_free(&inclusion_res);
  hash243_queue_free(&txs);
}
