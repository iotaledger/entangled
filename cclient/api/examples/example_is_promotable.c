/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TAIL_HASH =
    (tryte_t *)"9GKLL9R9YFXKBQRJNNGFSONCWRVDUJWQFYGWCTAAY9LWZMHEMAWVMIYYYKZXIIOZECKXBRWPEAUEGB999";

void example_is_promotable(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];
  bool is_promotable = false;

  if (flex_trits_from_trytes(trits_243, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
    return;
  }

  if ((ret = iota_client_is_promotable(s, trits_243, &is_promotable)) == RC_OK) {
    printf("promotable: %d \n", is_promotable);
  } else {
    printf("Error: %s \n", error_2_string(ret));
  }
}