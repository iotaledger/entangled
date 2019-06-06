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

void example_get_bundle(iota_client_service_t *s) {
  printf("\n [%s]\n", __FUNCTION__);
  retcode_t ret = RC_OK;
  flex_trit_t tail_hash[FLEX_TRIT_SIZE_243] = {};
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);

  if (flex_trits_from_trytes(tail_hash, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
  } else {
    if ((ret = iota_client_get_bundle(s, tail_hash, bundle, &bundle_status)) == RC_OK) {
      if (bundle_status == BUNDLE_VALID) {
        printf("bundle status: %d\n", bundle_status);
#ifdef DEBUG
        bundle_dump(bundle);
#endif
      } else {
        printf("Invalid bundle: %d\n", bundle_status);
      }
    } else {
      printf("Error: %s\n", error_2_string(ret));
    }
  }

  bundle_transactions_free(&bundle);
}