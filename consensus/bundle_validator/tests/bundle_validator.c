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

void test_iota_consensus_bundle_validator_validate_empty_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(&tangle, ep, bundle,
                                                       &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  trit_array_free(ep);
  bundle_transactions_free(&bundle);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_size_1_value_with_wrong_address_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];
  tryte_t tx1_trytes[] =
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999999999999999999999999999999999999999999999999"
      "999999999999999999999999999SQYSGYNQDUEMYK9RSEJXLBSGWBSEVIMZWQPHLMPKBAIJC"
      "TAEPDYLLRTESZSW9FMQYNSGQZKRKCXGNUWDOA99999999999999999999999999999999999"
      "999999999999999999RMADHWD99999999999D99999999GIXTMJIGESMVEKQNM9JOEUNO9CR"
      "QVIRTRVXOBV9ODB9MUAHOVAONAFHZLIQLWTQWTZUYGPEAORENMU9CBOFPLCLNEBUUIJBFHXK"
      "QTAQFCZZACGXQEGPTUUMGYXWSEFUK9MKTVJRPUUJESLUSN9FPRHMMLBRNG99999IHAVDVHYJ"
      "DKRYAOTUYNJCSNERFHACSDTCQIO9IPWSUGQN9DAVNJRUNLMZSRRFLUFLTRSGYVJLKOCW9999"
      "RDDGUFUSZZMJIGAHCWHWCNNBHSLYVCPBZOHGQQPMIPTWCMREXAQY9IIWOSYSUUWMBQOAUTXB"
      "Z9DOWWMPF";

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         tx1_trytes, NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  trit_array_free(tail_hash);
  trit_array_free(ep);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_exceed_supply_pos_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->value = IOTA_SUPPLY + 1;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  trit_array_free(tail_hash);
  trit_array_free(ep);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_exceed_supply_neg_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->value = -IOTA_SUPPLY - 1;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, TRANSACTION_FIELD_NONE,
                                            NULL, &exist) == RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  trit_array_free(tail_hash);
  trit_array_free(ep);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_wrong_sig_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  iota_transaction_t txs[4];

  tryte_t const *const trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  transactions_deserialize(trytes, txs, 4);
  trit_t buffer[NUM_TRITS_PER_FLEX_TRIT];
  flex_trits_to_trits(buffer, NUM_TRITS_PER_FLEX_TRIT,
                      txs[1]->signature_or_message, NUM_TRITS_PER_FLEX_TRIT,
                      NUM_TRITS_PER_FLEX_TRIT);
  buffer[NUM_TRITS_PER_FLEX_TRIT - 1] = 1;
  flex_trits_from_trits(&txs[1]->signature_or_message[FLEX_TRIT_SIZE_243 - 1],
                        NUM_TRITS_PER_FLEX_TRIT, buffer,
                        NUM_TRITS_PER_FLEX_TRIT, NUM_TRITS_PER_FLEX_TRIT);
  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, txs[0]->hash, NUM_TRITS_HASH);
  build_tangle(&tangle, txs, 4);

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);
  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
  transactions_free(txs, 4);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
}

void test_iota_consensus_bundle_validator_validate_size_4_value_valid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(iota_consensus_bundle_validator_init() == RC_OK);

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

  bool is_valid = false;

  TEST_ASSERT(iota_consensus_bundle_validator_validate(
                  &tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(is_valid);

  TEST_ASSERT(iota_consensus_bundle_validator_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transactions_free(txs, 4);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
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

  RUN_TEST(test_iota_consensus_bundle_validator_validate_empty_invalid);
  RUN_TEST(test_bundle_size_1_value_with_wrong_address_invalid);
  RUN_TEST(test_bundle_exceed_supply_pos_invalid);
  RUN_TEST(test_bundle_exceed_supply_neg_invalid);

  // FIXME (@tsvisabo) - ASAN yelling
  // RUN_TEST(test_bundle_validate_size_4_value_wrong_sig_invalid);

  RUN_TEST(test_iota_consensus_bundle_validator_validate_size_4_value_valid);

  return UNITY_END();
}
