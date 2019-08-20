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

#include "ciri/consensus/bundle_validator/bundle_validator.h"
#include "ciri/consensus/conf.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "common/model/transaction.h"
#include "common/storage/tests/helpers/defs.h"

static tangle_t tangle;
static connection_config_t config;

static char *tangle_test_db_path = "ciri/consensus/bundle_validator/tests/test.db";
static char *tangle_db_path = "common/storage/tangle.db";

void setUp() {
  TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);
}

void tearDown() {
  TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK);
  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
}

static void test_prepare_4txs(iota_transaction_t *txs[4]) {
  tryte_t const *const trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 4, true);
}

void test_iota_consensus_bundle_validator_validate_tail_not_found() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *test_tx = NULL;
  flex_trit_t tx_test_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(tx_test_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TEST_TX_TRYTES,
                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);

  test_tx = transaction_deserialize(tx_test_trits, true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(test_tx), bundle, &bundle_status) ==
              RC_TANGLE_TAIL_NOT_FOUND);
  TEST_ASSERT(bundle_status == BUNDLE_NOT_INITIALIZED);

  bundle_transactions_free(&bundle);
  transaction_free(test_tx);
}

void test_bundle_size_1_value_with_wrong_address_invalid() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  trit_t address[HASH_LENGTH_TRIT];
  bool exist = false;
  iota_transaction_t *txs[4];

  test_prepare_4txs(txs);
  flex_trits_to_trits(address, HASH_LENGTH_TRIT, transaction_address(txs[1]), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  address[HASH_LENGTH_TRIT - 1] = -1;
  flex_trits_from_trits(transaction_address(txs[0]), HASH_LENGTH_TRIT, address, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_INPUT_ADDRESS);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_exceed_supply_pos_invalid() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  transaction_set_value(txs[0], IOTA_SUPPLY + 1);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_VALUE);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_exceed_supply_neg_invalid() {
  bundle_transactions_t *bundle;
  iota_transaction_t *txs[4];
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  bool exist = false;

  test_prepare_4txs(txs);
  transaction_set_value(txs[0], -IOTA_SUPPLY - 1);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_VALUE);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_incomplete() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  TEST_ASSERT(build_tangle(&tangle, txs, 3) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INCOMPLETE);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_invalid_tx_with_wrong_current_idx() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  transaction_set_current_index(txs[1], 2);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_TX);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_invalid_tx_with_wrong_last_idx() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  transaction_set_last_index(txs[1], 2);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_TX);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_invalid_hash_with_wrong_address() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  trit_t address[HASH_LENGTH_TRIT];
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  flex_trits_to_trits(address, HASH_LENGTH_TRIT, transaction_address(txs[1]), HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  address[0] = -1;
  flex_trits_from_trits(transaction_address(txs[0]), HASH_LENGTH_TRIT, address, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_HASH);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_invalid_hash_with_wrong_obsolete_tag() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  trit_t obsolete_tag[NUM_TRITS_OBSOLETE_TAG];
  bool exist = false;

  test_prepare_4txs(txs);
  flex_trits_to_trits(obsolete_tag, NUM_TRITS_OBSOLETE_TAG, transaction_obsolete_tag(txs[1]), NUM_TRITS_OBSOLETE_TAG,
                      NUM_TRITS_OBSOLETE_TAG);
  obsolete_tag[0] = -1;
  flex_trits_from_trits(transaction_obsolete_tag(txs[0]), NUM_TRITS_OBSOLETE_TAG, obsolete_tag, NUM_TRITS_OBSOLETE_TAG,
                        NUM_TRITS_OBSOLETE_TAG);

  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_HASH);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_bundle_invalid_hash_with_wrong_transaction_timestamp() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  transaction_set_timestamp(txs[1], 2);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_HASH);

  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_wrong_sig_invalid() {
  bundle_transactions_t *bundle;
  iota_transaction_t *txs[4];
  trit_t buffer[NUM_TRITS_PER_FLEX_TRIT];
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;

  test_prepare_4txs(txs);
  flex_trits_to_trits(buffer, NUM_TRITS_PER_FLEX_TRIT, transaction_signature(txs[1]), NUM_TRITS_PER_FLEX_TRIT,
                      NUM_TRITS_PER_FLEX_TRIT);
  buffer[NUM_TRITS_PER_FLEX_TRIT - 1] = !buffer[NUM_TRITS_PER_FLEX_TRIT - 1];
  flex_trits_from_trits(transaction_signature(txs[1]), NUM_TRITS_PER_FLEX_TRIT, buffer, NUM_TRITS_PER_FLEX_TRIT,
                        NUM_TRITS_PER_FLEX_TRIT);

  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_INVALID_SIGNATURE);
  transactions_free(txs, 4);
  bundle_transactions_free(&bundle);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_valid() {
  bundle_transactions_t *bundle;
  bundle_status_t bundle_status = BUNDLE_NOT_INITIALIZED;
  iota_transaction_t *txs[4];
  bool exist = false;

  test_prepare_4txs(txs);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE, NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  bundle_transactions_new(&bundle);
  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, transaction_hash(txs[0]), bundle, &bundle_status) ==
              RC_OK);
  TEST_ASSERT(bundle_status == BUNDLE_VALID);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;

  RUN_TEST(test_iota_consensus_bundle_validator_validate_tail_not_found);
  RUN_TEST(test_bundle_size_1_value_with_wrong_address_invalid);
  RUN_TEST(test_bundle_exceed_supply_pos_invalid);
  RUN_TEST(test_bundle_exceed_supply_neg_invalid);
  RUN_TEST(test_bundle_incomplete);
  RUN_TEST(test_bundle_invalid_tx_with_wrong_current_idx);
  RUN_TEST(test_bundle_invalid_tx_with_wrong_last_idx);
  RUN_TEST(test_bundle_invalid_hash_with_wrong_address);
  RUN_TEST(test_bundle_invalid_hash_with_wrong_obsolete_tag);
  RUN_TEST(test_bundle_invalid_hash_with_wrong_transaction_timestamp);
  RUN_TEST(test_iota_consensus_bundle_validator_validate_size_4_value_wrong_sig_invalid);
  RUN_TEST(test_iota_consensus_bundle_validator_validate_size_4_value_valid);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
