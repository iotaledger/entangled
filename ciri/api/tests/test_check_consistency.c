/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "gossip/node.h"
#include "utils/files.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static core_t core;
static tangle_t tangle;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, test_db_path, ciri_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, test_db_path) == RC_OK); }

void test_check_consistency_invalid_subtangle_status(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_API_INVALID_SUBTANGLE_STATUS);
  TEST_ASSERT(res->state == false);

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
}

void test_check_consistency_missing_tail(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_1_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  hash243_queue_push(&req->tails, hash);

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_API_TAIL_MISSING);
  TEST_ASSERT(res->state == false);

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
}

void test_check_consistency_not_tail(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  iota_transaction_t tx;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_2_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  hash243_queue_push(&req->tails, hash);

  transaction_deserialize_from_trits(&tx, trits, true);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, &tx) == RC_OK);

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_API_NOT_TAIL);
  TEST_ASSERT(res->state == false);

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
}

void test_check_consistency_tail_not_solid(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  iota_transaction_t tx;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t trits[FLEX_TRIT_SIZE_8019];

  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_1_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  flex_trits_from_trytes(trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
                         NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  hash243_queue_push(&req->tails, hash);

  transaction_deserialize_from_trits(&tx, trits, true);
  TEST_ASSERT(iota_tangle_transaction_store(&tangle, &tx) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, hash, false) == RC_OK);

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_OK);
  TEST_ASSERT(res->state == false);
  TEST_ASSERT_EQUAL_STRING(res->info->data, API_TAILS_NOT_SOLID);

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
}

void test_check_consistency_invalid_bundle(void) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  iota_transaction_t *txs[4];
  tryte_t const *const trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  trit_t buffer[NUM_TRITS_PER_FLEX_TRIT];

  transactions_deserialize(trytes, txs, 4, true);
  flex_trits_to_trits(buffer, NUM_TRITS_PER_FLEX_TRIT, transaction_signature(txs[1]), NUM_TRITS_PER_FLEX_TRIT,
                      NUM_TRITS_PER_FLEX_TRIT);
  buffer[NUM_TRITS_PER_FLEX_TRIT - 1] = !buffer[NUM_TRITS_PER_FLEX_TRIT - 1];
  flex_trits_from_trits(transaction_signature(txs[1]), NUM_TRITS_PER_FLEX_TRIT, buffer, NUM_TRITS_PER_FLEX_TRIT,
                        NUM_TRITS_PER_FLEX_TRIT);
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_1_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, hash, true) == RC_OK);
  hash243_queue_push(&req->tails, hash);

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_OK);
  TEST_ASSERT(res->state == false);
  TEST_ASSERT_EQUAL_STRING(res->info->data, API_TAILS_BUNDLE_INVALID);

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
  transactions_free(txs, 4);
}

void test_check_consistency_consistent_ledger(bool consistency) {
  check_consistency_req_t *req = check_consistency_req_new();
  check_consistency_res_t *res = check_consistency_res_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  iota_transaction_t *txs[4];
  tryte_t const *const trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                    TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  tryte_t const *const hashes[4] = {TX_1_OF_4_HASH, TX_2_OF_4_HASH, TX_3_OF_4_HASH, TX_4_OF_4_HASH};

  transactions_deserialize(trytes, txs, 4, true);

  for (size_t i = 0; i < 4; i++) {
    memset(txs[i]->attachment.branch, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    if (i == 3) {
      memset(txs[i]->attachment.trunk, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
    }
  }

  TEST_ASSERT(build_tangle(&tangle, txs, 4) == RC_OK);
  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, hashes[i], HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(iota_tangle_transaction_update_snapshot_index(
                    &tangle, hash, api.core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index - 5) ==
                RC_OK);
  }
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_1_OF_4_HASH, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  hash243_queue_push(&req->tails, hash);
  TEST_ASSERT(iota_tangle_transaction_update_solid_state(&tangle, hash, true) == RC_OK);

  TEST_ASSERT(iota_api_check_consistency(&api, &tangle, req, res) == RC_OK);
  if (consistency) {
    TEST_ASSERT_TRUE(res->state);
    TEST_ASSERT_NULL(res->info);
  } else {
    TEST_ASSERT_FALSE(res->state);
    TEST_ASSERT_NOT_NULL(res->info);
    TEST_ASSERT_EQUAL_STRING(res->info->data, API_TAILS_NOT_CONSISTENT);
  }

  check_consistency_req_free(&req);
  check_consistency_res_free(&res);
  transactions_free(txs, 4);
}

void test_check_consistency_false(void) { test_check_consistency_consistent_ledger(false); }

void test_check_consistency_true(void) { test_check_consistency_consistent_ledger(true); }

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = test_db_path;
  api.core = &core;

  TEST_ASSERT(iota_gossip_conf_init(&api.core->node.conf) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(tips_cache_init(&api.core->node.tips, api.core->node.conf.tips_cache_size) == RC_OK);

  setUp();

  // Avoid verifying snapshot signature
  api.core->consensus.conf.snapshot_signature_file[0] = '\0';

  TEST_ASSERT(iota_consensus_init(&api.core->consensus, &tangle, &api.core->node.transaction_requester,
                                  &api.core->node.tips) == RC_OK);

  state_delta_destroy(&api.core->consensus.snapshot.state);

  tearDown();

  RUN_TEST(test_check_consistency_invalid_subtangle_status);

  api.core->consensus.milestone_tracker.latest_solid_subtangle_milestone_index++;

  RUN_TEST(test_check_consistency_missing_tail);
  RUN_TEST(test_check_consistency_not_tail);
  RUN_TEST(test_check_consistency_tail_not_solid);
  RUN_TEST(test_check_consistency_invalid_bundle);
  RUN_TEST(test_check_consistency_false);

  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(hash, HASH_LENGTH_TRIT, TX_2_OF_4_ADDRESS, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  state_delta_add(&api.core->consensus.snapshot.state, hash, 1545071560);

  RUN_TEST(test_check_consistency_true);

  TEST_ASSERT(iota_consensus_destroy(&api.core->consensus) == RC_OK);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
