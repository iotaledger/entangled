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

static const uint16_t mwm = 14;
static iota_consensus_conf_t conf;

void transaction_invalid_value() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, tx1->attachment_timestamp, mwm) == RC_OK);
  tx1->value = -IOTA_SUPPLY - 1;
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_hash() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, tx1->attachment_timestamp, mwm) == RC_OK);
  tx1->hash[FLEX_TRIT_SIZE_243 - 1] = tx1->hash[0];
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_attachment_timestamp_too_futuristic() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->attachment_timestamp = current_timestamp_ms() + 99 * 60 * 60 * 1000;
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf, 0, mwm) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_attachment_timestamp_too_old() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, current_timestamp_ms(), mwm) == RC_OK);
  tx1->hash[FLEX_TRIT_SIZE_243 - 1] = tx1->hash[0];
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_value_tx_wrong_address() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, tx1->attachment_timestamp, mwm) == RC_OK);
  tx1->address[FLEX_TRIT_SIZE_243 - 1] = tx1->address[4];
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_timestamp_too_futuristic() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->attachment_timestamp = 0;
  tx1->timestamp = current_timestamp_ms() + 99 * 60 * 60 * 1000;
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(&tv, &conf, 0, mwm) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_invalid_timestamp_too_old() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->attachment_timestamp = 0;
  uint64_t current_time = current_timestamp_ms();
  tx1->timestamp = current_time / 1000 - 1;
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, current_time, mwm) == RC_OK);
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1) == false);

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

void transaction_is_valid() {
  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  transaction_validator_t tv;
  TEST_ASSERT(iota_consensus_transaction_validator_init(
                  &tv, &conf, tx1->attachment_timestamp, mwm) == RC_OK);
  TEST_ASSERT(iota_consensus_transaction_validate(&tv, tx1));

  TEST_ASSERT(iota_consensus_transaction_validator_destroy(&tv) == RC_OK);
  transaction_free(tx1);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  memset(conf.genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);

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
