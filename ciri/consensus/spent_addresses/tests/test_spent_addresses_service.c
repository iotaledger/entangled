/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/consensus/spent_addresses/spent_addresses_service.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "utils/files.h"

static char *spent_addresses_test_db_path = "ciri/consensus/spent_addresses/tests/spent-addresses-test.db";
static char *spent_addresses_db_path = "common/storage/spent-addresses.db";
static char *tangle_test_db_path = "ciri/consensus/spent_addresses/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static storage_connection_config_t spent_addresses_config;
static storage_connection_config_t tangle_config;
static iota_consensus_conf_t consensus_conf;
static spent_addresses_provider_t sap;
static spent_addresses_service_t sas;
static tangle_t tangle;

void setUp(void) {
  TEST_ASSERT(iota_utils_copy_file(spent_addresses_test_db_path, spent_addresses_db_path) == RC_OK);
  TEST_ASSERT(iota_spent_addresses_provider_init(&sap, &spent_addresses_config) == RC_OK);
  TEST_ASSERT(tangle_setup(&tangle, &tangle_config, tangle_test_db_path, tangle_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(iota_spent_addresses_provider_destroy(&sap) == RC_OK);
  TEST_ASSERT(iota_utils_remove_file(spent_addresses_test_db_path) == RC_OK);
  TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK);
}

static void test_genesis_not_spent() {
  bool spent = true;
  flex_trit_t genesis[FLEX_TRIT_SIZE_243];

  memset(genesis, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, genesis, &spent) == RC_OK);
  TEST_ASSERT_FALSE(spent);
}

static void test_coordinator_not_spent() {
  bool spent = true;
  flex_trit_t coordinator[FLEX_TRIT_SIZE_243];

  memcpy(coordinator, consensus_conf.coordinator_address, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, coordinator, &spent) == RC_OK);
  TEST_ASSERT_FALSE(spent);
}

static void test_spent_in_provider() {
  bool spent = false;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, 'I', HASH_LENGTH_TRYTE);
  flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT(iota_spent_addresses_provider_store(&sap, address_trits) == RC_OK);

  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, address_trits, &spent) == RC_OK);
  TEST_ASSERT_TRUE(spent);
}

static void test_tx_no_spend_tx() {
  bool spent = true;
  iota_transaction_t *txs[1];
  tryte_t const *const txs_trytes[1] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(txs_trytes, txs, 1, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 1) == RC_OK);

  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, transaction_address(txs[0]),
                                                                  &spent) == RC_OK);
  TEST_ASSERT_FALSE(spent);

  transactions_free(txs, 1);
}

static void test_tx_spend_tx_confirmed() {
  bool spent = false;
  iota_transaction_t *txs[1];
  tryte_t const *const txs_trytes[1] = {TX_2_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(txs_trytes, txs, 1, true);
  transaction_set_snapshot_index(txs[0], 42);
  TEST_ASSERT(build_tangle(&tangle, txs, 1) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(&tangle, transaction_hash(txs[0]), 42) == RC_OK);

  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, transaction_address(txs[0]),
                                                                  &spent) == RC_OK);
  TEST_ASSERT_TRUE(spent);

  transactions_free(txs, 1);
}

static void test_valid_bundle() {
  bool spent = false;
  bool exist = true;
  iota_transaction_t *txs[4];
  tryte_t const *const trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};

  transactions_deserialize(trytes, txs, 4, true);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);

  TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, transaction_address(txs[1]), &exist) == RC_OK);
  TEST_ASSERT_FALSE(exist);
  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, transaction_address(txs[1]),
                                                                  &spent) == RC_OK);
  TEST_ASSERT_TRUE(spent);
  transactions_free(txs, 4);
}

static void test_not_spent() {
  bool spent = true;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, 'I', HASH_LENGTH_TRYTE);
  flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);

  TEST_ASSERT(iota_spent_addresses_service_was_address_spent_from(&sas, &sap, &tangle, address_trits, &spent) == RC_OK);
  TEST_ASSERT_FALSE(spent);
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  spent_addresses_config.db_path = spent_addresses_test_db_path;
  tangle_config.db_path = tangle_test_db_path;

  TEST_ASSERT(iota_consensus_conf_init(&consensus_conf) == RC_OK);

  TEST_ASSERT(iota_spent_addresses_service_init(&sas, &consensus_conf) == RC_OK);

  RUN_TEST(test_genesis_not_spent);
  RUN_TEST(test_coordinator_not_spent);
  RUN_TEST(test_spent_in_provider);
  RUN_TEST(test_tx_no_spend_tx);
  RUN_TEST(test_tx_spend_tx_confirmed);
  RUN_TEST(test_valid_bundle);
  RUN_TEST(test_not_spent);

  TEST_ASSERT(iota_spent_addresses_service_destroy(&sas) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
