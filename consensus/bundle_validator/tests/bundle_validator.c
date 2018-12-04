/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "unity/unity.h"
#include "utarray.h"

#include "common/model/transaction.h"
#include "common/storage/tests/helpers/defs.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/conf.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static tangle_t tangle;
static connection_config_t config;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char *test_db_path = "consensus/bundle_validator/tests/test.db";
static char *ciri_db_path = "consensus/bundle_validator/tests/ciri.db";

void setUp() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);
  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);
}

void tearDown() {
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
}

void test_iota_consensus_bundle_validator_validate_tail_not_found() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  trit_array_p tail = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_TAIL_NOT_FOUND);

  trit_array_free(tail);
  bundle_transactions_free(&bundle);
}

void test_bundle_size_1_value_with_wrong_address_invalid() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);
  trit_t address[HASH_LENGTH_TRIT];

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(trytes, txs, 4);
  flex_trits_to_trits(address, HASH_LENGTH_TRIT, txs[1]->address,
                      HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  address[HASH_LENGTH_TRIT - 1] = -1;
  flex_trits_from_trits(txs[0]->address, HASH_LENGTH_TRIT, address,
                        HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  build_tangle(&tangle, txs, 4);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_INPUT_ADDRESS);

  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_exceed_supply_pos_invalid() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(trytes, txs, 4);
  txs[0]->value = IOTA_SUPPLY + 1;
  build_tangle(&tangle, txs, 4);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_VALUE);

  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_exceed_supply_neg_invalid() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(trytes, txs, 4);
  txs[0]->value = -IOTA_SUPPLY - 1;
  build_tangle(&tangle, txs, 4);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_VALUE);

  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_wrong_sig_invalid() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 4);
  trit_t buffer[NUM_TRITS_PER_FLEX_TRIT];
  flex_trits_to_trits(buffer, NUM_TRITS_PER_FLEX_TRIT,
                      (flex_trit_t *)&txs[1]->signature_or_message,
                      NUM_TRITS_PER_FLEX_TRIT, NUM_TRITS_PER_FLEX_TRIT);
  buffer[NUM_TRITS_PER_FLEX_TRIT - 1] = !buffer[NUM_TRITS_PER_FLEX_TRIT - 1];
  flex_trits_from_trits((flex_trit_t *)&txs[1]->signature_or_message,
                        NUM_TRITS_PER_FLEX_TRIT, buffer,
                        NUM_TRITS_PER_FLEX_TRIT, NUM_TRITS_PER_FLEX_TRIT);
  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);
  build_tangle(&tangle, txs, 4);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_SIGNATURE);
  transactions_free(txs, 4);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_valid() {
  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 4);
  build_tangle(&tangle, txs, 4);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);

  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &bundle_status) == RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_VALID);

  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  if (argc >= 2) {
    debug_mode = true;
  }
  if (debug_mode) {
    test_db_path = "test.db";
    ciri_db_path = "ciri.db";
  }

  config.db_path = test_db_path;

  RUN_TEST(test_iota_consensus_bundle_validator_validate_tail_not_found);
  RUN_TEST(test_bundle_size_1_value_with_wrong_address_invalid);
  RUN_TEST(test_bundle_exceed_supply_pos_invalid);
  RUN_TEST(test_bundle_exceed_supply_neg_invalid);
  RUN_TEST(
      test_iota_consensus_bundle_validator_validate_size_4_value_wrong_sig_invalid);
  RUN_TEST(test_iota_consensus_bundle_validator_validate_size_4_value_valid);

  return UNITY_END();
}
