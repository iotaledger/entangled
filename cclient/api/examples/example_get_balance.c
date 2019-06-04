/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const ADDR_HASH =
    (tryte_t *)"XUERGHWTYRTFUYKFKXURKHMFEVLOIFTTCNTXOGLDPCZ9CJLKHROOPGNAQYFJEPGK9OKUQROUECBAVNXRX";

void example_get_balance(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_OK;
  flex_trit_t tmp_hash[FLEX_TRIT_SIZE_243];
  get_balances_req_t *balance_req = get_balances_req_new();
  get_balances_res_t *balance_res = get_balances_res_new();

  if (!balance_req || !balance_res) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(tmp_hash, NUM_TRITS_HASH, ADDR_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  if ((ret_code = hash243_queue_push(&balance_req->addresses, tmp_hash)) != RC_OK) {
    printf("Error: Adding hash to list failed!\n");
    goto done;
  }

  balance_req->threshold = 100;

  if ((ret_code = iota_client_get_balances(s, balance_req, balance_res)) == RC_OK) {
    hash243_queue_entry_t *q_iter = NULL;
    size_t balance_cnt = get_balances_res_balances_num(balance_res);
    printf("balances: [");
    for (size_t i = 0; i < balance_cnt; i++) {
      printf(" %" PRIu64 " ", get_balances_res_balances_at(balance_res, i));
    }
    printf("]\n");

    CDL_FOREACH(balance_res->references, q_iter) {
      printf("reference: ");
      flex_trit_print(q_iter->hash, NUM_TRITS_HASH);
      printf("\n");
    }
  }

done:
  get_balances_req_free(&balance_req);
  get_balances_res_free(&balance_res);
}