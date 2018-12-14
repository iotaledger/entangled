/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static iota_consensus_t consensus;

// Compares two queues by using an intermediary hash set
static bool hash243_queue_cmp(hash243_queue_t const lhs,
                              hash243_queue_t const rhs) {
  hash243_set_t set = NULL;
  hash243_queue_entry_t *iter = NULL;
  bool cmp = true;

  if (hash243_queue_count(lhs) != hash243_queue_count(rhs)) {
    cmp = false;
    goto done;
  }

  CDL_FOREACH(lhs, iter) { hash243_set_add(&set, iter->hash); }

  CDL_FOREACH(rhs, iter) {
    if (!hash243_set_contains(&set, iter->hash)) {
      cmp = false;
      goto done;
    }
  }

done:
  hash243_set_free(&set);
  return cmp;
}

void setUp(void) {
  TEST_ASSERT(tangle_setup(&consensus.tangle, &config, test_db_path,
                           ciri_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&consensus.tangle, test_db_path) == RC_OK);
}

/**
 * In this test, we store 24 transactions distributed in 8 bundles.
 * Each transaction is labeled by the 1st char of its hash: from A to X.
 * Each bundle is labeled by the 2nd char of its hash: from A to H.
 * We then request all transactions in bundles B, E and G.
 * We should end up with 9 transactions: B, E, G, J, M, O, R, U, W.
 */
void test_find_transactions_only_bundles(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  hash243_queue_t res_hashes = NULL;

  struct _iota_transaction txs[24];

  tryte_t hash_trytes[HASH_LENGTH_TRYTE];
  memcpy(hash_trytes, NULL_HASH, HASH_LENGTH_TRYTE);
  hash_trytes[0] = 'A';

  tryte_t bundle_trytes[HASH_LENGTH_TRYTE];
  memcpy(bundle_trytes, NULL_HASH, HASH_LENGTH_TRYTE);
  bundle_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, HASH_LENGTH_TRIT, hash_trytes,
                           HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    flex_trits_from_trytes(txs[i].essence.bundle, HASH_LENGTH_TRIT,
                           bundle_trytes, HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
    TEST_ASSERT(iota_tangle_transaction_store(&consensus.tangle, &txs[i]) ==
                RC_OK);
    if (bundle_trytes[1] == 'B' || bundle_trytes[1] == 'E' ||
        bundle_trytes[1] == 'G') {
      hash243_queue_push(&res_hashes, hash_trytes);
    }
    hash_trytes[0]++;
    bundle_trytes[1] = 'A' + (i + 1) % 8;
  }

  bundle_trytes[1] = 'B';
  hash243_queue_push(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'E';
  hash243_queue_push(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'G';
  hash243_queue_push(&req->bundles, bundle_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, req, res) == RC_OK);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  hash243_queue_free(&res_hashes);
}

void test_find_transactions_no_input(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();

  TEST_ASSERT(iota_api_find_transactions(&api, req, res) ==
              RC_API_FIND_TRANSACTIONS_NO_INPUT);

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.consensus = &consensus;
  api.conf.max_find_transactions = 1024;

  RUN_TEST(test_find_transactions_only_bundles);
  RUN_TEST(test_find_transactions_no_input);

  return UNITY_END();
}
