/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/consensus/test_utils/bundle.h"

static iota_api_t api;

void test_attach_to_tangle(void) {
  attach_to_tangle_req_t *req = attach_to_tangle_req_new();
  attach_to_tangle_res_t *res = attach_to_tangle_res_new();
  error_res_t *error = NULL;
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  tryte_t const *const trunk =
      (tryte_t *)"QBDYOWCHXVFNDEFXJXIBYZLWVHYISYUWRIDFNXZDTQTSNSCPOIMB9NIW9GEY9DZWMCPBFCVEBROZ99999";
  tryte_t const *const branch =
      (tryte_t *)"A9VNT9ZZXOFMUAAR9SKZITFCXVGXPFMDLH9DCNZOCQCNNEWZBAQCOJHFIIIOSDHNZ9YKLUYVZKFW99999";
  uint8_t mwm = 10;
  flex_trit_t flex_trunk[FLEX_TRIT_SIZE_243];
  flex_trit_t flex_branch[FLEX_TRIT_SIZE_243];
  flex_trit_t *trytes_iter = NULL;
  iota_transaction_t tx;
  int i = 0;
  flex_trit_t prev_trunk[FLEX_TRIT_SIZE_243];

  flex_trits_from_trytes(flex_trunk, NUM_TRITS_TRUNK, trunk, NUM_TRYTES_TRUNK, NUM_TRYTES_TRUNK);
  flex_trits_from_trytes(flex_branch, NUM_TRITS_BRANCH, branch, NUM_TRYTES_BRANCH, NUM_TRYTES_BRANCH);

  attach_to_tangle_req_init(req, flex_trunk, flex_branch, mwm);

  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, txs_trytes[i], NUM_TRYTES_SERIALIZED_TRANSACTION,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
    TEST_ASSERT(attach_to_tangle_req_trytes_add(req, tx_trits) == RC_OK);
  }

  TEST_ASSERT(iota_api_attach_to_tangle(&api, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(hash_array_len(res->trytes), 4);

  HASH_ARRAY_FOREACH(res->trytes, trytes_iter) {
    transaction_deserialize_from_trits(&tx, trytes_iter, true);
    TEST_ASSERT(transaction_weight_magnitude(&tx) >= mwm);
    if (i != 3) {
      if (i != 0) {
        TEST_ASSERT_EQUAL_MEMORY(transaction_hash(&tx), prev_trunk, FLEX_TRIT_SIZE_243);
      }
      TEST_ASSERT_EQUAL_MEMORY(transaction_branch(&tx), flex_trunk, FLEX_TRIT_SIZE_243);
      memcpy(prev_trunk, transaction_trunk(&tx), FLEX_TRIT_SIZE_243);
    } else {
      TEST_ASSERT_EQUAL_MEMORY(transaction_hash(&tx), prev_trunk, FLEX_TRIT_SIZE_243);
      TEST_ASSERT_EQUAL_MEMORY(transaction_trunk(&tx), flex_trunk, FLEX_TRIT_SIZE_243);
      TEST_ASSERT_EQUAL_MEMORY(transaction_branch(&tx), flex_branch, FLEX_TRIT_SIZE_243);
    }
    i++;
  }

  attach_to_tangle_req_free(&req);
  attach_to_tangle_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_attach_to_tangle);

  return UNITY_END();
}
