/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TEST_HASH =
    (tryte_t *)"OAATQS9VQLSXCLDJVJJVYUGONXAXOFMJOZNSYWRZSWECMXAQQURHQBJNLD9IOFEPGZEPEMPXCIVRX9999";

void example_check_consistency(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_OK;
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  check_consistency_req_t *consistency_req = check_consistency_req_new();
  check_consistency_res_t *consistency_res = check_consistency_res_new();
  if (!consistency_req || !consistency_res) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TEST_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed\n");
    goto done;
  }

  hash243_queue_push(&consistency_req->tails, trits_243);

  if ((ret = iota_client_check_consistency(s, consistency_req, consistency_res)) == RC_OK) {
    printf("%s\n", consistency_res->state ? "true" : "false");
    printf("%s\n", consistency_res->info->data);
  } else {
    printf("Error: %s\n", error_2_string(ret));
  }

done:
  check_consistency_req_free(&consistency_req);
  check_consistency_res_free(&consistency_res);
}