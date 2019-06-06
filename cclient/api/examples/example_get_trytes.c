/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const GET_TRYTES_HASH =
    (tryte_t *)"KFJKAEPEVABBATAZKSDEKZS9KTIMS9CRELLBLLODHDDWRSWUBFAGHTQXEWCJQUEBQKVVNDLPUENMMR999";

void example_get_trytes(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_OK;
  flex_trit_t tmp_hash[FLEX_TRIT_SIZE_243];
  get_trytes_req_t *trytes_req = get_trytes_req_new();
  get_trytes_res_t *trytes_res = get_trytes_res_new();

  if (!trytes_req || !trytes_res) {
    printf("Error OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(tmp_hash, NUM_TRITS_HASH, GET_TRYTES_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if (hash243_queue_push(&trytes_req->hashes, tmp_hash) != RC_OK) {
    printf("Error: %s\n", error_2_string(ret_code));
    goto done;
  }

  if ((ret_code = iota_client_get_trytes(s, trytes_req, trytes_res)) == RC_OK) {
    hash8019_queue_entry_t *q_iter = NULL;
    CDL_FOREACH(trytes_res->trytes, q_iter) {
      flex_trit_print(q_iter->hash, NUM_TRITS_SERIALIZED_TRANSACTION);
      printf("\n");
    }
  } else {
    printf("Error: %s\n", error_2_string(ret_code));
  }

done:
  get_trytes_res_free(&trytes_res);
  get_trytes_req_free(&trytes_req);
}