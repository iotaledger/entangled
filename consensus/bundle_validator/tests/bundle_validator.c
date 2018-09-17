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
#include "common/storage/connection.h"
#include "common/storage/sql/defs.h"
#include "common/storage/storage.h"
#include "common/storage/tests/helpers/defs.h"
#include "common/trinary/flex_trit.h"
#include "consensus/bundle_validator/bundle_validator.h"
#include "consensus/defs.h"
#include "consensus/tangle/tangle.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "utarray.h"

static tangle_t tangle;
static connection_config_t config;

// gdb --args ./test_cw_ratings_dfs 1
static bool debug_mode = false;

static char *test_db_path = "consensus/bundle_validator/tests/test.db";
static char *ciri_db_path = "consensus/bundle_validator/tests/ciri.db";

void test_bundle_validate_empty_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, (flex_trit_t *)TEST_TRANSACTION.hash,
                       NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, ep, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(ep);
  bundle_transactions_free(&bundle);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_size_1_value_with_wrong_address_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);

  iota_transaction_t tx1 = transaction_deserialize(
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
      "Z9DOWWMPF");

  iota_transaction_t curr_tx = NULL;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx1 = &curr_tx_s;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx1), 1, 0, false};

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_exceed_supply_pos_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->value = IOTA_SUPPLY + 1;

  iota_transaction_t curr_tx = NULL;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx1 = &curr_tx_s;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx1), 1, 0, false};

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_exceed_supply_neg_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  tx1->value = -IOTA_SUPPLY - 1;

  iota_transaction_t curr_tx = NULL;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);

  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx1 = &curr_tx_s;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx1), 1, 0, false};

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_validate_size_4_value_wrong_sig_invalid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_2_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_3_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_4_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_2_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_2_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_3_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_3_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_4_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_4_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  iota_transaction_t tx2 = transaction_deserialize(transaction_2_trits);
  iota_transaction_t tx3 = transaction_deserialize(transaction_3_trits);
  iota_transaction_t tx4 = transaction_deserialize(transaction_4_trits);

  tx2->signature_or_message[0] = 'A';

  iota_transaction_t curr_tx = NULL;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx2) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx3) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx4) == RC_OK);

  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx1 = &curr_tx_s;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx1), 1, 0, false};

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(!is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  transaction_free(tx2);
  transaction_free(tx3);
  transaction_free(tx4);
  TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK);
}

void test_bundle_validate_size_4_value_valid() {
  TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) ==
              RC_OK);

  TEST_ASSERT(bundle_validate_init() == RC_OK);

  bundle_transactions_t *bundle;
  bundle_transactions_new(&bundle);

  flex_trit_t transaction_1_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_2_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_3_trits[FLEX_TRIT_SIZE_8019];
  flex_trit_t transaction_4_trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(transaction_1_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_1_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_2_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_2_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_3_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_3_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);
  flex_trits_from_trytes(transaction_4_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                         TX_4_OF_4_VALUE_BUNDLE_TRYTES, FLEX_TRIT_SIZE_8019,
                         FLEX_TRIT_SIZE_8019);

  iota_transaction_t tx1 = transaction_deserialize(transaction_1_trits);
  iota_transaction_t tx2 = transaction_deserialize(transaction_2_trits);
  iota_transaction_t tx3 = transaction_deserialize(transaction_3_trits);
  iota_transaction_t tx4 = transaction_deserialize(transaction_4_trits);

  iota_transaction_t curr_tx = NULL;

  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx1) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx2) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx3) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, tx4) == RC_OK);

  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx1 = &curr_tx_s;

  iota_stor_pack_t tx_pack = {(void **)(&curr_tx1), 1, 0, false};

  trit_array_p ep = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(ep, tx1->trunk, NUM_TRITS_HASH);

  bool exist = false;
  TEST_ASSERT(iota_tangle_transaction_exist(&tangle, NULL, NULL, &exist) ==
              RC_OK);
  TEST_ASSERT(exist == true);

  trit_array_p tail_hash = trit_array_new(NUM_TRITS_HASH);
  trit_array_set_trits(tail_hash, tx1->hash, NUM_TRITS_HASH);

  bool is_valid;

  TEST_ASSERT(bundle_validate(&tangle, tail_hash, bundle, &is_valid) == RC_OK);
  TEST_ASSERT(is_valid);

  TEST_ASSERT(bundle_validate_destroy() == RC_OK);
  trit_array_free(tail_hash);
  bundle_transactions_free(&bundle);
  transaction_free(tx1);
  transaction_free(tx2);
  transaction_free(tx3);
  transaction_free(tx4);
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
  config.index_transaction_address = true;
  config.index_transaction_approvee = true;
  config.index_transaction_bundle = true;
  config.index_transaction_tag = true;
  config.index_transaction_hash = true;
  config.index_milestone_hash = true;

  RUN_TEST(test_bundle_validate_empty_invalid);
  RUN_TEST(test_bundle_size_1_value_with_wrong_address_invalid);
  RUN_TEST(test_bundle_exceed_supply_pos_invalid);
  RUN_TEST(test_bundle_exceed_supply_neg_invalid);
  RUN_TEST(test_bundle_validate_size_4_value_wrong_sig_invalid);

  RUN_TEST(test_bundle_validate_size_4_value_valid);

  return UNITY_END();
}
