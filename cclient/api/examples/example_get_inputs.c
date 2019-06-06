/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_get_inputs(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_OK;
  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  address_opt_t opt = {.security = 2, .start = 0, .total = 0};

  if (flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS) == 0) {
    printf("Error: converting flex_trit failed\n");
    return;
  }

  // new inputs
  inputs_t inputs = {};
  input_t *in = NULL;

  if ((ret = iota_client_get_inputs(s, seed, opt, 2000, &inputs)) == RC_OK) {
    INPUTS_FOREACH(inputs.input_array, in) {
      printf("[%" PRIu64 "] ", in->balance);
      flex_trit_print(in->address, NUM_TRITS_ADDRESS);
      printf("\n");
    }
    printf("total = %" PRIu64 "\n", inputs.total_balance);

  } else {
    printf("Error: %s\n", error_2_string(ret));
  }

  inputs_clear(&inputs);
}