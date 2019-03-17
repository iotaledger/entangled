/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "common/model/transaction.h"
#include "consensus/conf.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/transaction_validator/transaction_validator.h"
#include "utarray.h"
#include "utils/time.h"

static iota_consensus_conf_t conf;

void transaction_invalid_value() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = transaction_attachment_timestamp(tx1) / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  transaction_set_value(tx1, -IOTA_SUPPLY - 1);
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_hash() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = transaction_attachment_timestamp(tx1) / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  tx1->consensus.hash[FLEX_TRIT_SIZE_243 - 1] = tx1->consensus.hash[0];
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_attachment_timestamp_too_futuristic() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_set_attachment_timestamp(tx1, current_timestamp_ms() + 99 * 60 * 60 * 1000);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = 0;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_attachment_timestamp_too_old() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = current_timestamp_ms() / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  tx1->consensus.hash[FLEX_TRIT_SIZE_243 - 1] = tx1->consensus.hash[0];
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_value_tx_wrong_address() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = transaction_attachment_timestamp(tx1) / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  transaction_address(tx1)[FLEX_TRIT_SIZE_243 - 1] = transaction_address(tx1)[4];
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_timestamp_too_futuristic() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_set_attachment_timestamp(tx1, 0);
  transaction_set_timestamp(tx1, current_timestamp_ms() + 99 * 60 * 60 * 1000);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = 0;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_timestamp_too_old() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_set_attachment_timestamp(tx1, 0);
  uint64_t current_time = current_timestamp_ms();
  transaction_set_timestamp(tx1, current_time / 1000 - 1);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = current_time / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  TEST_ASSERT_FALSE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_is_valid() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t *tx1 = transaction_deserialize(transaction_1_trits, true);
  transaction_validator_t tv;
  conf.snapshot_timestamp_sec = transaction_attachment_timestamp(tx1) / 1000;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf) == RC_OK);
  TEST_ASSERT_TRUE(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  iota_consensus_conf_init(&conf);
  conf.mwm = 14;

  RUN_TEST(transaction_is_valid);
  RUN_TEST(transaction_invalid_value);
  RUN_TEST(transaction_invalid_hash);
  RUN_TEST(transaction_invalid_attachment_timestamp_too_futuristic);
  RUN_TEST(transaction_invalid_attachment_timestamp_too_old);
  RUN_TEST(transaction_invalid_value_tx_wrong_address);
  RUN_TEST(transaction_invalid_timestamp_too_futuristic);
  RUN_TEST(transaction_invalid_timestamp_too_old);

  return UNITY_END();
}
