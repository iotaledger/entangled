/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const RECEIVER_ADDR =
    (tryte_t *)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB";

void example_prepare_transfer(iota_client_service_t *s) {
  printf("\n [%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_ERROR;
  transfer_array_t *transfers = transfer_array_new();
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  transfer_t tf = {};

  // my seed
  flex_trit_t seed[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  // receiver
  flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, RECEIVER_ADDR, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, (const tryte_t *)"CCLIENT99999999999999999999", NUM_TRYTES_TAG,
                         NUM_TRYTES_TAG);

  transfer_message_set_string(&tf, "Hello IOTA");
  tf.value = 1;  // send 1i out
  transfer_array_add(transfers, &tf);

#if 1  // no remainder and inputs
  ret_code = iota_client_prepare_transfers(s, seed, 2, transfers, NULL, NULL, false, 0, bundle);
#else  // no inputs
  flex_trit_t remainder_addr[FLEX_TRIT_SIZE_243];
  ret_code = iota_client_prepare_transfers(s, seed, 2, transfers, remainder_addr, NULL, false, 0, bundle);
#endif

#if 0  // send with input
  inputs_t input_list = {};
  input_t input_a = {
      .balance = 1996,
      .key_index = 6,
      .security = 2,
      .address = {},
  };
  flex_trits_from_trytes(
      input_a.address, NUM_TRITS_ADDRESS,
      (const tryte_t *)"UKMUJQSTTBGMXMEYUHZMWTECCPDQEKSHKUNZCF9WUATDHZWWPUKDRLOLTAVMALMYSLMZIMWKOHPLVNPNX",
      NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  inputs_append(&input_list, &input_a);
  ret_code = iota_client_prepare_transfers(s, seed, 2, transfers, NULL, &input_list, true, 0, bundle);
  inputs_clear(&input_list);
#endif

  if (ret_code == RC_OK) {
    printf("%s done\n", __FUNCTION__);
#ifdef DEBUG
    bundle_dump(bundle);
#endif
  } else {
    printf("Error: %s\n", error_2_string(ret_code));
  }

  transfer_array_free(transfers);
  bundle_transactions_free(&bundle);
  transfer_message_free(&tf);
}
