/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const REF_HASH =
    (tryte_t *)"VOPPIXFBHNNEFAAZJLLKTTFMMVJZPHQCTROSU99KYHYBGLJYMCPGIQGSUW9NQQ9TOVTEOKFDCAJLXA999";

void example_get_transactions_to_approve(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  flex_trit_t reference[FLEX_TRIT_SIZE_243];
  get_transactions_to_approve_req_t *tx_approve_req = get_transactions_to_approve_req_new();
  get_transactions_to_approve_res_t *tx_approve_res = get_transactions_to_approve_res_new();

  if (!tx_approve_req || !tx_approve_res) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(reference, NUM_TRITS_HASH, REF_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret = get_transactions_to_approve_req_set_reference(tx_approve_req, reference)) != RC_OK) {
    printf("Error: OOM on setting reference\n");
    goto done;
  }

  tx_approve_req->depth = 9;  // 14 for mainnet

  if ((ret = iota_client_get_transactions_to_approve(s, tx_approve_req, tx_approve_res)) == RC_OK) {
    printf("trunk: ");
    flex_trit_print(tx_approve_res->trunk, NUM_TRITS_HASH);
    printf("\n");

    printf("branch: ");
    flex_trit_print(tx_approve_res->branch, NUM_TRITS_HASH);
    printf("\n");
  } else {
    printf("Error: %s\n", error_2_string(ret));
  }

done:
  get_transactions_to_approve_req_free(&tx_approve_req);
  get_transactions_to_approve_res_free(&tx_approve_res);
}