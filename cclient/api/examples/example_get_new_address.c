/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_get_new_address(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_ERROR;
  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  hash243_queue_t addresses = NULL;
  // address_opt_t opt = {.security = 2, .start = 0, .total = 5}; //get five addresses.
  address_opt_t opt = {.security = 2, .start = 0, .total = 0};  // get an unused address and all used addresses.

  if (flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS) == 0) {
    printf("Error: converting flex_trit failed\n");
    return;
  }

  if ((ret = iota_client_get_new_address(s, seed, opt, &addresses)) == RC_OK) {
    printf("unused: ");
    flex_trit_print(addresses->prev->hash, NUM_TRITS_ADDRESS);
    printf("\n");

    size_t count = hash243_queue_count(addresses);
    hash243_queue_t curr = addresses;
    for (size_t i = 0; i < count; i++) {
      printf("[%ld]: ", i);
      flex_trit_print(curr->hash, NUM_TRITS_ADDRESS);
      printf("\n");
      curr = curr->next;
    }
  } else {
    printf("new address failed: %s\n", error_2_string(ret));
  }
  hash243_queue_free(&addresses);
}