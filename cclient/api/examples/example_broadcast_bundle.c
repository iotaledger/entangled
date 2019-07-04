/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TAIL_HASH =
    (tryte_t *)"UVQZIWPHJZUBVLSFKXWALZZCNIMMK9UAGWMXYCLTKZLXMWV9EHAAAKFOOHOWANEKRAJXHEJFYK9AYO999";

void example_broadcast_bundle(iota_client_service_t *s) {
  retcode_t ret = RC_OK;
  flex_trit_t tail_hash[FLEX_TRIT_SIZE_243] = {};

  if (flex_trits_from_trytes(tail_hash, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
  } else {
    bundle_transactions_t *bundle = NULL;
    bundle_transactions_new(&bundle);

    if ((ret = iota_client_broadcast_bundle(s, tail_hash, bundle)) == RC_OK) {
      printf("broadcast bundle done...\n");
#ifdef DEBUG
      bundle_dump(bundle);
#endif
    } else {
      printf("Error: %s\n", error_2_string(ret));
    }

    bundle_transactions_free(&bundle);
  }
}
