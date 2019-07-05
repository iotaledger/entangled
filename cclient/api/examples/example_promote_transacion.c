/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

static tryte_t const *const TAIL_HASH =
    (tryte_t *)"EN9KKJOJTTDFZRKERWV99IXFDPERTZSWVMOERJSLUGUHBPMLAIHTJTTMKQRWZQTYKPWZYZCDETJY99999";

void example_promote_transaction(iota_client_service_t *s) {
  retcode_t ret = RC_OK;
  bundle_transactions_t *bundle = NULL;
  flex_trit_t tail_hash[FLEX_TRIT_SIZE_243] = {};
  bundle_transactions_new(&bundle);
  if (bundle == NULL) {
    printf("Error: OOM\n");
    goto done;
  }

  if (flex_trits_from_trytes(tail_hash, NUM_TRITS_HASH, TAIL_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
    goto done;
  } else {
    transfer_array_t *spam_stransfer = transfer_array_new();
    transfer_t tf = {};
    tf.value = 0;
    memset(tf.address, '9', NUM_TRYTES_ADDRESS);
    memset(tf.tag, '9', NUM_TRYTES_TAG);
    transfer_array_add(spam_stransfer, &tf);
    if ((ret = iota_client_promote_transaction(s, tail_hash, 2, 3, 9, spam_stransfer, bundle)) == RC_OK) {
      printf("transactions has been promoted\n");
#ifdef DEBUG
      bundle_dump(bundle);
#endif
    } else {
      printf("Error: %s\n", error_2_string(ret));
    }
    transfer_array_free(spam_stransfer);
  }

done:
  bundle_transactions_free(&bundle);
}
