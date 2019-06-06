/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/api/examples/cclient_examples.h"

#define FIND_BY_ADDR

static tryte_t const *const ADDR_HASH =
    (tryte_t *)"XUERGHWTYRTFUYKFKXURKHMFEVLOIFTTCNTXOGLDPCZ9CJLKHROOPGNAQYFJEPGK9OKUQROUECBAVNXRX";

void example_find_transaction_objects(iota_client_service_t *s) {
  printf("\n[%s]\n", __FUNCTION__);
  retcode_t ret_code = RC_ERROR;
  find_transactions_req_t *find_tran = find_transactions_req_new();
  transaction_array_t *out_tx_objs = transaction_array_new();
  if (!find_tran || !out_tx_objs) {
    printf("Error: OOM\n");
    goto done;
  }

#ifdef FIND_BY_ADDR
  // find transaction by address
  flex_trit_t tmp_hash[FLEX_TRIT_SIZE_243];
  if (flex_trits_from_trytes(tmp_hash, NUM_TRITS_HASH, ADDR_HASH, NUM_TRYTES_HASH, NUM_TRYTES_HASH) == 0) {
    printf("Error: converting flex_trit failed.\n");
    goto done;
  }

  if ((ret_code = hash243_queue_push(&find_tran->addresses, tmp_hash)) != RC_OK) {
    printf("Error: push queue %s\n", error_2_string(ret_code));
    goto done;
  }
#else
  flex_trit_t tmp_tag[FLEX_TRIT_SIZE_81];
  // find transaction by tag
  if (flex_trits_from_trytes(tmp_tag, NUM_TRITS_TAG, (const tryte_t *)"CCLIENT99999999999999999999", NUM_TRYTES_TAG,
                             NUM_TRYTES_TAG) == 0) {
    printf("Error: converting flex_trit failed.\n");
    goto done;
  }
  if ((ret_code = hash81_queue_push(&find_tran->tags, tmp_tag)) != RC_OK) {
    printf("Error: push queue %s\n", error_2_string(ret_code));
    goto done;
  }
#endif
  if ((ret_code = iota_client_find_transaction_objects(s, find_tran, out_tx_objs)) == RC_OK) {
    printf("find transaction count: %lu\n", transaction_array_len(out_tx_objs));
    iota_transaction_t *tx1 = transaction_array_at(out_tx_objs, 1);
    if (tx1) {
      printf("dump first transaction:\n");
      printf("value = %" PRId64 ", curr_index = %" PRIu64 ", last_index = %" PRIu64 "\n", transaction_value(tx1),
             transaction_current_index(tx1), transaction_last_index(tx1));
      printf("addr: ");
      flex_trit_print(transaction_address(tx1), NUM_TRITS_ADDRESS);
      printf("\n");
    }
  } else {
    printf("Error: %s \n", error_2_string(ret_code));
  }

done:
  find_transactions_req_free(&find_tran);
  transaction_array_free(out_tx_objs);
}