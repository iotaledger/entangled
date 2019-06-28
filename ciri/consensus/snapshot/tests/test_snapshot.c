/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/consensus/snapshot/snapshot.h"
#include "common/model/transaction.h"

static char *snapshot_conf_path = "ciri/consensus/snapshot/tests/snapshot_conf.json";

snapshot_t snapshot;
iota_consensus_conf_t conf;

void test_snapshot_conf() {
  tryte_t sig_pubkey[HASH_LENGTH_TRYTE];
  tryte_t coo[HASH_LENGTH_TRYTE];

  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  TEST_ASSERT(iota_snapshot_conf_init(&conf) == RC_OK);
  TEST_ASSERT_EQUAL_INT(conf.snapshot_timestamp_sec, 1537203600UL);
  TEST_ASSERT_EQUAL_INT(conf.snapshot_signature_index, 9UL);
  TEST_ASSERT_EQUAL_INT(conf.snapshot_signature_depth, 6UL);
  TEST_ASSERT_EQUAL_INT(conf.last_milestone, 774804UL);
  flex_trits_to_trytes(sig_pubkey, HASH_LENGTH_TRYTE, conf.snapshot_signature_pubkey, HASH_LENGTH_TRIT,
                       HASH_LENGTH_TRIT);
  flex_trits_to_trytes(coo, HASH_LENGTH_TRYTE, conf.coordinator_address, HASH_LENGTH_TRIT, HASH_LENGTH_TRIT);
  TEST_ASSERT_EQUAL_MEMORY(
      sig_pubkey, (tryte_t *)"TTXJUGKTNPOOEXSTQVVACENJOQUROXYKDRCVK9LHUXILCLABLGJTIPNF9REWHOIMEUKWQLUOKD9CZUYAC",
      NUM_TRYTES_ADDRESS);
  TEST_ASSERT_EQUAL_MEMORY(
      coo, (tryte_t *)"KPWCHICGJZXKE9GSUDXZYUAPLHAKAHYHDXNPHENTERYMMBQOPSQIDENXKLKCEYCPVTZQLEEJVYJZV9BWU",
      NUM_TRYTES_ADDRESS);
}

void test_snapshot_init_file_not_found() {
  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot_not_found.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_UTILS_FILE_DOES_NOT_EXITS);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_badly_formatted() {
  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot_badly_formatted.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_SNAPSHOT_INVALID_FILE);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_inconsistent() {
  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot_inconsistent.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_SNAPSHOT_INCONSISTENT_SNAPSHOT);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_init_file_invalid_supply() {
  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot_invalid_supply.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_SNAPSHOT_INVALID_SUPPLY);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_check_consistency() {
  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_OK);
  TEST_ASSERT(state_delta_is_consistent(&snapshot.state) == true);
  snapshot.state->value *= -1;
  TEST_ASSERT(state_delta_is_consistent(&snapshot.state) == false);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_get_balance() {
  flex_trit_t address[FLEX_TRIT_SIZE_243];
  int64_t balance;

  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_OK);
  flex_trits_from_trytes(address, NUM_TRITS_HASH,
                         (tryte_t*)"J9999999999999999999999999999999999999999999999999999"
                         "9999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, address, &balance) == RC_OK);
  flex_trits_from_trytes(address, NUM_TRITS_HASH,
                         (tryte_t*)"Z9999999999999999999999999999999999999999999999999999"
                         "9999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, address, &balance) == RC_SNAPSHOT_BALANCE_NOT_FOUND);
  TEST_ASSERT_EQUAL_INT(balance, 3000000);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
}

void test_snapshot_create_and_apply_patch() {
  state_delta_t delta = NULL;
  state_delta_t patch = NULL;
  int64_t balance = 0;
  flex_trit_t hash1[FLEX_TRIT_SIZE_243];
  flex_trit_t hash2[FLEX_TRIT_SIZE_243];

  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_OK);
  flex_trits_from_trytes(hash1, NUM_TRITS_HASH,
                         (tryte_t *)"O99999999999999999999999999999999999999999999999999999999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(state_delta_add(&delta, hash1, (int64_t)-50) == RC_OK);
  TEST_ASSERT(iota_snapshot_create_patch(&snapshot, &delta, &patch) == RC_OK);
  TEST_ASSERT(iota_snapshot_apply_patch(&snapshot, &delta, 1) == RC_SNAPSHOT_INCONSISTENT_PATCH);
  state_delta_destroy(&patch);
  flex_trits_from_trytes(hash2, NUM_TRITS_HASH,
                         (tryte_t *)"Q99999999999999999999999999999999999999999999999999999999999999999999999999999999",
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(state_delta_add(&delta, hash2, (int64_t)50) == RC_OK);
  TEST_ASSERT(iota_snapshot_create_patch(&snapshot, &delta, &patch) == RC_OK);
  TEST_ASSERT(iota_snapshot_apply_patch(&snapshot, &delta, 2) == RC_OK);
  state_delta_destroy(&patch);
  TEST_ASSERT_EQUAL_INT(iota_snapshot_get_index(&snapshot), 2);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, hash1, &balance) == RC_OK);
  TEST_ASSERT_EQUAL_INT(balance, 10);
  TEST_ASSERT(iota_snapshot_get_balance(&snapshot, hash2, &balance) == RC_OK);
  TEST_ASSERT_EQUAL_INT(balance, 50);
  TEST_ASSERT(iota_snapshot_destroy(&snapshot) == RC_OK);
  state_delta_destroy(&delta);
}

int main() {
  UNITY_BEGIN();

  iota_consensus_conf_init(&conf);
  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  conf.snapshot_signature_skip_validation = true;
  conf.local_snapshots.local_snapshots_is_enabled = false;

  RUN_TEST(test_snapshot_conf);
  RUN_TEST(test_snapshot_init_file_not_found);
  RUN_TEST(test_snapshot_init_file_badly_formatted);
  RUN_TEST(test_snapshot_init_file_inconsistent);
  RUN_TEST(test_snapshot_init_file_invalid_supply);
  RUN_TEST(test_snapshot_check_consistency);
  RUN_TEST(test_snapshot_get_balance);
  RUN_TEST(test_snapshot_create_and_apply_patch);

  return UNITY_END();
}
