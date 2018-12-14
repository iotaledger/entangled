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
 * In this test, we store 24 txs distributed in 8 bundles of 3 txs.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each bundle is labeled by the 2nd char of its hash: from A to H.
 * We then request all txs in bundles B, E and G.
 * We should end up with 9 txs: B, E, G, J, M, O, R, U, W.
 */
void test_find_transactions_bundles_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  hash243_queue_t res_hashes = NULL;

  struct _iota_transaction txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t bundle_trytes[NUM_TRYTES_BUNDLE];
  memcpy(bundle_trytes, NULL_HASH, NUM_TRYTES_BUNDLE);
  bundle_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes,
                           NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.bundle, NUM_TRITS_BUNDLE,
                           bundle_trytes, NUM_TRYTES_BUNDLE, NUM_TRYTES_BUNDLE);
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

/**
 * In this test, we store 24 txs where 6 addresses receive/send 4 txs each.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each address is labeled by the 2nd char of its hash: from A to F.
 * We then request all txs received/sent by/to addresses A, D and F.
 * We should end up with 12 txs: A, D, F, G, J, L, M, P, R, S, T, X.
 */
void test_find_transactions_addresses_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  hash243_queue_t res_hashes = NULL;

  struct _iota_transaction txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t address_trytes[NUM_TRYTES_ADDRESS];
  memcpy(address_trytes, NULL_HASH, NUM_TRYTES_ADDRESS);
  address_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes,
                           NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.address, NUM_TRITS_ADDRESS,
                           address_trytes, NUM_TRYTES_ADDRESS,
                           NUM_TRYTES_ADDRESS);
    TEST_ASSERT(iota_tangle_transaction_store(&consensus.tangle, &txs[i]) ==
                RC_OK);
    if (address_trytes[1] == 'A' || address_trytes[1] == 'D' ||
        address_trytes[1] == 'F') {
      hash243_queue_push(&res_hashes, hash_trytes);
    }
    hash_trytes[0]++;
    address_trytes[1] = 'A' + (i + 1) % 6;
  }

  address_trytes[1] = 'A';
  hash243_queue_push(&req->addresses, address_trytes);
  address_trytes[1] = 'D';
  hash243_queue_push(&req->addresses, address_trytes);
  address_trytes[1] = 'F';
  hash243_queue_push(&req->addresses, address_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, req, res) == RC_OK);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  hash243_queue_free(&res_hashes);
}

/**
 * In this test, we store 24 txs with 6 txs for 4 different tags.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each tag is labeled by the 2nd char of its hash: from A to D.
 * We then request all txs with tag A or C.
 * We should end up with 12 txs: A, C, E, G, I, K, M, O, Q, S, U, W.
 */
void test_find_transactions_tags_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  hash243_queue_t res_hashes = NULL;

  struct _iota_transaction txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t tags_trytes[NUM_TRYTES_TAG];
  memcpy(tags_trytes, NULL_HASH, NUM_TRYTES_TAG);
  tags_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes,
                           NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].attachment.tag, NUM_TRITS_TAG, tags_trytes,
                           NUM_TRYTES_TAG, NUM_TRYTES_TAG);
    TEST_ASSERT(iota_tangle_transaction_store(&consensus.tangle, &txs[i]) ==
                RC_OK);
    if (tags_trytes[1] == 'A' || tags_trytes[1] == 'C') {
      hash243_queue_push(&res_hashes, hash_trytes);
    }
    hash_trytes[0]++;
    tags_trytes[1] = 'A' + (i + 1) % 4;
  }

  tags_trytes[1] = 'A';
  hash81_queue_push(&req->tags, tags_trytes);
  tags_trytes[1] = 'C';
  hash81_queue_push(&req->tags, tags_trytes);

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

  RUN_TEST(test_find_transactions_bundles_only);
  RUN_TEST(test_find_transactions_addresses_only);
  RUN_TEST(test_find_transactions_tags_only);
  RUN_TEST(test_find_transactions_no_input);

  return UNITY_END();
}
