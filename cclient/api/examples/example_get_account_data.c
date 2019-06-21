/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

void example_get_account_data(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret = RC_OK;
  flex_trit_t seed[FLEX_TRIT_SIZE_243];

  if (flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS) == 0) {
    printf("Error: converting flex_trit failed\n");
    return;
  }

  // init account data
  account_data_t account = {};
  account_data_init(&account);

  if ((ret = iota_client_get_account_data(s, seed, 2, &account)) == RC_OK) {
#if 1  // dump transaction hashes
    size_t tx_count = hash243_queue_count(account.transactions);
    for (size_t i = 0; i < tx_count; i++) {
      printf("[%zu]: ", i);
      flex_trit_print(hash243_queue_at(account.transactions, i), NUM_TRITS_ADDRESS);
      printf("\n");
    }
    printf("transaction count %zu\n", tx_count);
#endif

    // dump balance
    printf("total balance: %" PRIu64 "\n", account.balance);

    // dump unused address
    printf("unused address: ");
    flex_trit_print(account.latest_address, NUM_TRITS_ADDRESS);
    printf("\n");

    // dump addresses
    size_t addr_count = hash243_queue_count(account.addresses);
    printf("address count %zu\n", addr_count);
    for (size_t i = 0; i < addr_count; i++) {
      printf("[%ld] ", i);
      flex_trit_print(hash243_queue_at(account.addresses, i), NUM_TRITS_ADDRESS);
      printf(" : %" PRIu64 "\n", account_data_get_balance(&account, i));
    }

    account_data_clear(&account);
  } else {
    printf("Error: %s\n", error_2_string(ret));
  }
}
