/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const RECEIVER_ADDR =
    (tryte_t *)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB";

void example_send_balance(iota_client_service_t *s) {
  printf("\n [%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_OK;
  uint32_t depth = 6;
  uint8_t mwm = 9;
  uint8_t security = 2;
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  transfer_array_t *transfers = transfer_array_new();

  /* transfer setup */
  transfer_t tf = {};
  // seed
  flex_trit_t seed[NUM_FLEX_TRITS_ADDRESS];
  flex_trits_from_trytes(seed, NUM_TRITS_ADDRESS, MY_SEED, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  // receiver
  flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, RECEIVER_ADDR, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  // tag
  flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, (const tryte_t *)"CCLIENT99999999999999999999", NUM_TRYTES_TAG,
                         NUM_TRYTES_TAG);

  // value
  tf.value = 1;  // send 5i to receiver

  // message (optional)
  transfer_message_set_string(&tf, "Sending 1i!!");

  transfer_array_add(transfers, &tf);
#if 0  // send with input options
  /* input setup (optional) */
  inputs_t input_list = {};  // input list
  input_t input_a = {
      .balance = 1, .key_index = 1, .security = 2, .address = {},  // set address later
  };

  // address of the input
  flex_trits_from_trytes(
      input_a.address, NUM_TRITS_ADDRESS,
      (const tryte_t *)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB",
      NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  // adding input object to the input list
  inputs_append(&input_list, &input_a);

#if 0
  /* reminder address (optional) */
  flex_trit_t reminder_addr[NUM_FLEX_TRITS_ADDRESS];
  flex_trits_from_trytes(
      seed, NUM_TRITS_ADDRESS,
      (const tryte_t *)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB",
      NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);
#else
  flex_trit_t *reminder_addr = NULL;
  flex_trit_t *reference = NULL;
#endif

  ret_code = iota_client_send_transfer(s, seed, security, depth, mwm, false, transfers, reminder_addr, reference,
                                       &input_list, bundle);
  inputs_clear(&input_list);
#else
  ret_code = iota_client_send_transfer(s, seed, security, depth, mwm, false, transfers, NULL, NULL, NULL, bundle);
#endif

  printf("send transfer %s\n", error_2_string(ret_code));
  if (ret_code == RC_OK) {
    flex_trit_t const *bundle_hash = bundle_transactions_bundle_hash(bundle);
    printf("bundle hash: ");
    flex_trit_print(bundle_hash, NUM_TRITS_HASH);
    printf("\n");
  }
#ifdef DEBUG
  bundle_dump(bundle);
#endif
  bundle_transactions_free(&bundle);
  transfer_message_free(&tf);
  transfer_array_free(transfers);
}