/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/consensus/spent_addresses/spent_addresses_provider.h"
#include "utils/files.h"

static char *spent_addresses_test_db_path = "ciri/consensus/spent_addresses/tests/test.db";
static char *spent_addresses_db_path = "common/storage/spent-addresses.db";
static connection_config_t config;
static spent_addresses_provider_t sap;

void setUp(void) {
  TEST_ASSERT(iota_utils_copy_file(spent_addresses_test_db_path, spent_addresses_db_path) == RC_OK);
  TEST_ASSERT(iota_spent_addresses_provider_init(&sap, &config) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(iota_spent_addresses_provider_destroy(&sap) == RC_OK);
  TEST_ASSERT(iota_utils_remove_file(spent_addresses_test_db_path) == RC_OK);
}

static void test_spent_addresses_provider_store() {
  bool exist = true;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      exist = false;
      TEST_ASSERT(iota_spent_addresses_provider_store(&sap, address_trits) == RC_OK);
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_TRUE(exist);
    } else {
      exist = true;
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_FALSE(exist);
    }
    address_trytes[0]++;
  }
}

static void test_spent_addresses_provider_batch_store() {
  bool exist = true;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];
  hash243_set_t addresses = NULL;

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      TEST_ASSERT(hash243_set_add(&addresses, address_trits) == RC_OK);
    }
    address_trytes[0]++;
  }

  TEST_ASSERT(iota_spent_addresses_provider_batch_store(&sap, addresses) == RC_OK);

  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      exist = false;
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_TRUE(exist);
    } else {
      exist = true;
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_FALSE(exist);
    }
    address_trytes[0]++;
  }

  hash243_set_free(&addresses);
}

static void test_spent_addresses_provider_import() {
  bool exist = true;
  tryte_t address_trytes[HASH_LENGTH_TRYTE];
  flex_trit_t address_trits[FLEX_TRIT_SIZE_243];

  memset(address_trytes, '9', HASH_LENGTH_TRYTE);
  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    exist = true;
    TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
    TEST_ASSERT_FALSE(exist);
    address_trytes[0]++;
  }

  TEST_ASSERT(iota_spent_addresses_provider_import(
                  &sap, "ciri/consensus/spent_addresses/tests/spent_addresses_test.txt") == RC_OK);

  address_trytes[0] = 'A';

  for (size_t i = 0; i < 26; i++) {
    flex_trits_from_trytes(address_trits, HASH_LENGTH_TRIT, address_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    if (i % 2) {
      exist = false;
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_TRUE(exist);
    } else {
      exist = true;
      TEST_ASSERT(iota_spent_addresses_provider_exist(&sap, address_trits, &exist) == RC_OK);
      TEST_ASSERT_FALSE(exist);
    }
    address_trytes[0]++;
  }
}

int main() {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = spent_addresses_test_db_path;

  RUN_TEST(test_spent_addresses_provider_store);
  RUN_TEST(test_spent_addresses_provider_batch_store);
  RUN_TEST(test_spent_addresses_provider_import);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
