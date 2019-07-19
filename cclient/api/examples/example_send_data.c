/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

#if 1  // short message
static char const *const message = "Hello IOTA CClient!";
#else  // long message
static char const *const message =
    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nulla condimentum diam nisi, aliquam tempor risus ornare "
    "a. Phasellus dignissim orci eget sagittis ultricies. Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
    "Duis facilisis turpis eu sem egestas tempus. Pellentesque a efficitur dui. Nullam vulputate euismod enim, nec "
    "vehicula augue venenatis vel. Vivamus tempor mollis dui a sollicitudin. Duis egestas lorem id elit vulputate, sit "
    "amet commodo odio cursus. Donec ac dictum lacus, a varius turpis. Proin scelerisque magna vitae tempor facilisis. "
    "Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Etiam id dui "
    "ultricies ligula faucibus eleifend et sed dolor. Duis aliquet lectus justo, at sodales ex vehicula ac. Integer "
    "luctus nulla et est luctus, sit amet lobortis ante pharetra.Sed sit amet ex tristique, faucibus felis et, "
    "dignissim elit. Sed vitae mollis lacus. Proin nec rutrum arcu. Vestibulum nec velit ac risus rutrum aliquet vitae "
    "dignissim est. Praesent non ante vitae est ultricies tempus. Nullam erat tellus, eleifend ut rutrum eget, "
    "sagittis a tortor. Morbi iaculis blandit laoreet. Quisque non lectus id ex pharetra fringilla id vitae mi. Nunc "
    "pharetra sed sapien ac blandit. Sed vulputate risus eget magna vehicula, quis facilisis quam vestibulum.Donec "
    "odio risus, maximus non elit in, mollis pharetra mi. Quisque lobortis cursus pretium. Pellentesque ac bibendum "
    "sem. Integer non leo placerat, mollis ex sit amet, dapibus sed.";
#endif

static tryte_t const *const RECEIVER_ADDR =
    (tryte_t *)"RJBYLCIOUKWJVCUKZQZCPIKNBUOGRGVXHRTTE9ZFSCGTFRKELMJBDDAKEYYCLHLJDNSHQ9RTIUIDLMUOB";

void example_send_data(iota_client_service_t *s) {
  printf("\n [%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_OK;
  int depth = 6;
  int mwm = 9;
  uint8_t security = 2;
  bundle_transactions_t *bundle = NULL;
  bundle_transactions_new(&bundle);
  transfer_array_t *transfers = transfer_array_new();

  /* transfer setup */
  transfer_t tf = {};

  // receiver
  flex_trits_from_trytes(tf.address, NUM_TRITS_ADDRESS, RECEIVER_ADDR, NUM_TRYTES_ADDRESS, NUM_TRYTES_ADDRESS);

  // tag
  flex_trits_from_trytes(tf.tag, NUM_TRITS_TAG, (const tryte_t *)"CCLIENT99999999999999999999", NUM_TRYTES_TAG,
                         NUM_TRYTES_TAG);

  // message
  transfer_message_set_string(&tf, message);

  transfer_array_add(transfers, &tf);

  // the seed can be NULL when sending zero tx.
  ret_code = iota_client_send_transfer(s, NULL, security, depth, mwm, false, transfers, NULL, NULL, NULL, bundle);

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