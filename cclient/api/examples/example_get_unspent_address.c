/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_get_unspent_address(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  uint64_t unspent_index = 0;
  flex_trit_t unspent_addr[NUM_FLEX_TRITS_ADDRESS];
  memset(unspent_addr, FLEX_TRIT_NULL_VALUE, NUM_FLEX_TRITS_ADDRESS);

  // try five addresses
  // address_opt_t opt = {.security = 2, .start = 0, .total = 5};
  // try all addresses
  address_opt_t opt = {.security = 2, .start = 0, .total = UINT64_MAX};

  if (flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS) == 0) {
    printf("Error: converting flex_trit failed\n");
    return;
  }

  if ((ret = iota_client_get_unspent_address(s, seed, opt, unspent_addr, &unspent_index)) == RC_OK) {
    if (flex_trits_are_null(unspent_addr, NUM_FLEX_TRITS_ADDRESS)) {
      printf("Cannot find an unspent address!\n");
    } else {
      printf("unspent[%" PRIu64 "]: ", unspent_index);
      flex_trit_print(unspent_addr, NUM_TRITS_ADDRESS);
      printf("\n");
    }
  } else {
    printf("new address failed: %s\n", error_2_string(ret));
  }
}