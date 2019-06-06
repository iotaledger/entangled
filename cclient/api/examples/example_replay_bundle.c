/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

#if 1  // BUNDLE_VALID
static tryte_t const *const TAIL_HASH =
    (tryte_t *)"ELMDITVNWJKQGNQZCHTFPPLNVS9UYLZR9BDNTPXWSNKOTIMWGENZZ9ZQXLMRNHGFHUYQKHV9PTJTIG999";
#else  // BUNDLE_INVALID_SIGNATURE
static tryte_t const *const TAIL_HASH =
    (tryte_t *)"VSOWHIZZSRQZVFJLYVOJSWUMPZNJAQUOHYEYCZCFBPXZIZWFMDCYWXZEPOGKJQIDLJIZJRQPHYMSA9999";
#endif

void example_replay_bundle(iota_client_service_t *s) {
  printf("\n [%s]\n", __FUNCTION__);
  retcode_t ret = RC_OK;
  flex_trit_t tail_hash[FLEX_TRIT_SIZE_243] = {};
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  if (flex_trits_from_trytes(tail_hash, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
  } else {
    if ((ret = iota_client_replay_bundle(s, tail_hash, 6, 9, NULL, bundle)) == RC_OK) {
      printf("%s done\n", __FUNCTION__);
#ifdef DEBUG
      bundle_dump(bundle);
#endif
    } else {
      printf("Error: %s\n", error_2_string(ret));
    }
  }
  bundle_transactions_free(&bundle);
}