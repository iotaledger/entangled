/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static storage_connection_config_t config;
static iota_api_t api;
static tangle_t tangle;

// Compares two queues by using an intermediary hash set
static bool hash243_queue_cmp(hash243_queue_t const lhs, hash243_queue_t const rhs) {
  hash243_set_t set = NULL;
  hash243_queue_entry_t *iter = NULL;
  bool cmp = true;

  if (hash243_queue_count(lhs) != hash243_queue_count(rhs)) {
    cmp = false;
    goto done;
  }

  CDL_FOREACH(lhs, iter) { hash243_set_add(&set, iter->hash); }

  CDL_FOREACH(rhs, iter) {
    if (!hash243_set_contains(set, iter->hash)) {
      cmp = false;
      goto done;
    }
  }

done:
  hash243_set_free(&set);
  return cmp;
}

static void hash243_queue_push_trytes(hash243_queue_t *const queue, tryte_t const *const trytes) {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  hash243_queue_push(queue, hash);
}

static void hash81_queue_push_trytes(hash81_queue_t *const queue, tryte_t const *const trytes) {
  flex_trit_t hash[FLEX_TRIT_SIZE_81];
  flex_trits_from_trytes(hash, NUM_TRITS_TAG, trytes, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  hash81_queue_push(queue, hash);
}

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

/**
 * In this test, we store 24 txs distributed in 8 bundles of 3 txs.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each bundle is labeled by the 2nd char of its hash: from A to H.
 * We then request all txs in bundles B, E and G.
 * We should end up with 9 txs: B, E, G, J, M, O, R, U and W.
 */
void test_find_transactions_bundles_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;
  hash243_queue_t res_hashes = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t bundle_trytes[NUM_TRYTES_BUNDLE];
  memcpy(bundle_trytes, NULL_HASH, NUM_TRYTES_BUNDLE);
  bundle_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.bundle, NUM_TRITS_BUNDLE, bundle_trytes, NUM_TRYTES_BUNDLE,
                           NUM_TRYTES_BUNDLE);
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    if (bundle_trytes[1] == 'B' || bundle_trytes[1] == 'E' || bundle_trytes[1] == 'G') {
      hash243_queue_push(&res_hashes, txs[i].consensus.hash);
    }
    hash_trytes[0]++;
    bundle_trytes[1] = 'A' + (i + 1) % 8;
  }

  bundle_trytes[1] = 'B';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'E';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'G';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
  hash243_queue_free(&res_hashes);
}

/**
 * In this test, we store 24 txs where 6 addresses receive/send 4 txs each.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each address is labeled by the 2nd char of its hash: from A to F.
 * We then request all txs received/sent by/to addresses A, D and F.
 * We should end up with 12 txs: A, D, F, G, J, L, M, P, R, S, T and X.
 */
void test_find_transactions_addresses_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;
  hash243_queue_t res_hashes = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t address_trytes[NUM_TRYTES_ADDRESS];
  memcpy(address_trytes, NULL_HASH, NUM_TRYTES_ADDRESS);
  address_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.address, NUM_TRITS_ADDRESS, address_trytes, NUM_TRYTES_ADDRESS,
                           NUM_TRYTES_ADDRESS);
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    if (address_trytes[1] == 'A' || address_trytes[1] == 'D' || address_trytes[1] == 'F') {
      hash243_queue_push(&res_hashes, txs[i].consensus.hash);
    }
    hash_trytes[0]++;
    address_trytes[1] = 'A' + (i + 1) % 6;
  }

  address_trytes[1] = 'A';
  hash243_queue_push_trytes(&req->addresses, address_trytes);
  address_trytes[1] = 'D';
  hash243_queue_push_trytes(&req->addresses, address_trytes);
  address_trytes[1] = 'F';
  hash243_queue_push_trytes(&req->addresses, address_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
  hash243_queue_free(&res_hashes);
}

/**
 * In this test, we store 24 txs with 6 txs for 4 different tags.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each tag is labeled by the 2nd char of its hash: from A to D.
 * We then request all txs with tags A or C.
 * We should end up with 12 txs: A, C, E, G, I, K, M, O, Q, S, U and W.
 */
void test_find_transactions_tags_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;
  hash243_queue_t res_hashes = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t tags_trytes[NUM_TRYTES_TAG];
  memcpy(tags_trytes, NULL_HASH, NUM_TRYTES_TAG);
  tags_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].attachment.tag, NUM_TRITS_TAG, tags_trytes, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    if (tags_trytes[1] == 'A' || tags_trytes[1] == 'C') {
      hash243_queue_push(&res_hashes, txs[i].consensus.hash);
    }
    hash_trytes[0]++;
    tags_trytes[1] = 'A' + (i + 1) % 4;
  }

  tags_trytes[1] = 'A';
  hash81_queue_push_trytes(&req->tags, tags_trytes);
  tags_trytes[1] = 'C';
  hash81_queue_push_trytes(&req->tags, tags_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
  hash243_queue_free(&res_hashes);
}

/**
 * In this test, we store 24 txs that approve a set of 16 different txs.
 * Each tx is labeled by the 1st char of its hash: from A to X.
 * Each approvee is labeled by the 2nd char of its hash: from A to P.
 * We then request all txs with approvees B, H, C or M.
 * We should end up with 12 txs: A, B, D, G, I, J, L, O, Q, R, T and W.
 */
void test_find_transactions_approvees_only(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;
  hash243_queue_t res_hashes = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t approvee_trytes[NUM_TRYTES_BRANCH];
  memcpy(approvee_trytes, NULL_HASH, NUM_TRYTES_BRANCH);
  approvee_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].attachment.branch, NUM_TRITS_BRANCH, approvee_trytes, NUM_TRYTES_BRANCH,
                           NUM_TRYTES_BRANCH);
    if (approvee_trytes[1] == 'B' || approvee_trytes[1] == 'C' || approvee_trytes[1] == 'H' ||
        approvee_trytes[1] == 'M') {
      hash243_queue_push(&res_hashes, txs[i].consensus.hash);
    }
    approvee_trytes[1] = 'A' + (2 * i + 1) % 16;
    flex_trits_from_trytes(txs[i].attachment.trunk, NUM_TRITS_BRANCH, approvee_trytes, NUM_TRYTES_BRANCH,
                           NUM_TRYTES_BRANCH);
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    if (approvee_trytes[1] == 'B' || approvee_trytes[1] == 'C' || approvee_trytes[1] == 'H' ||
        approvee_trytes[1] == 'M') {
      hash243_queue_push(&res_hashes, txs[i].consensus.hash);
    }
    hash_trytes[0]++;
    approvee_trytes[1] = 'A' + (2 * i + 2) % 16;
  }

  approvee_trytes[1] = 'B';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'C';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'H';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'M';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT(hash243_queue_cmp(res->hashes, res_hashes));

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
  hash243_queue_free(&res_hashes);
}

/*
 * This test is the intersection of the 4 previous tests.
 * We should end up with only one tx: G.
 */
void test_find_transactions_intersection(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t bundle_trytes[NUM_TRYTES_BUNDLE];
  memcpy(bundle_trytes, NULL_HASH, NUM_TRYTES_BUNDLE);
  bundle_trytes[1] = 'A';

  tryte_t address_trytes[NUM_TRYTES_ADDRESS];
  memcpy(address_trytes, NULL_HASH, NUM_TRYTES_ADDRESS);
  address_trytes[1] = 'A';

  tryte_t tags_trytes[NUM_TRYTES_TAG];
  memcpy(tags_trytes, NULL_HASH, NUM_TRYTES_TAG);
  tags_trytes[1] = 'A';

  tryte_t approvee_trytes[NUM_TRYTES_BRANCH];
  memcpy(approvee_trytes, NULL_HASH, NUM_TRYTES_BRANCH);
  approvee_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.bundle, NUM_TRITS_BUNDLE, bundle_trytes, NUM_TRYTES_BUNDLE,
                           NUM_TRYTES_BUNDLE);
    flex_trits_from_trytes(txs[i].essence.address, NUM_TRITS_ADDRESS, address_trytes, NUM_TRYTES_ADDRESS,
                           NUM_TRYTES_ADDRESS);
    flex_trits_from_trytes(txs[i].attachment.tag, NUM_TRITS_TAG, tags_trytes, NUM_TRYTES_TAG, NUM_TRYTES_TAG);
    flex_trits_from_trytes(txs[i].attachment.branch, NUM_TRITS_BRANCH, approvee_trytes, NUM_TRYTES_BRANCH,
                           NUM_TRYTES_BRANCH);

    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);

    hash_trytes[0]++;
    bundle_trytes[1] = 'A' + (i + 1) % 8;
    address_trytes[1] = 'A' + (i + 1) % 6;
    tags_trytes[1] = 'A' + (i + 1) % 4;
    approvee_trytes[1] = 'A' + (2 * i + 2) % 16;
  }

  bundle_trytes[1] = 'B';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'E';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);
  bundle_trytes[1] = 'G';
  hash243_queue_push_trytes(&req->bundles, bundle_trytes);

  address_trytes[1] = 'A';
  hash243_queue_push_trytes(&req->addresses, address_trytes);
  address_trytes[1] = 'D';
  hash243_queue_push_trytes(&req->addresses, address_trytes);
  address_trytes[1] = 'F';
  hash243_queue_push_trytes(&req->addresses, address_trytes);

  tags_trytes[1] = 'A';
  hash81_queue_push_trytes(&req->tags, tags_trytes);
  tags_trytes[1] = 'C';
  hash81_queue_push_trytes(&req->tags, tags_trytes);

  approvee_trytes[1] = 'B';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'C';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'H';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);
  approvee_trytes[1] = 'M';
  hash243_queue_push_trytes(&req->approvees, approvee_trytes);

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  hash_trytes[0] = 'G';
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_INT(hash243_queue_count(res->hashes), 1);
  TEST_ASSERT_EQUAL_MEMORY(hash, hash243_queue_peek(res->hashes), FLEX_TRIT_SIZE_243);

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
}

void test_find_transactions_no_input(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_API_FIND_TRANSACTIONS_NO_INPUT);
  TEST_ASSERT(error == NULL);

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
}

void test_find_transactions_max(void) {
  find_transactions_req_t *req = find_transactions_req_new();
  find_transactions_res_t *res = find_transactions_res_new();
  error_res_t *error = NULL;

  iota_transaction_t txs[24];

  tryte_t hash_trytes[NUM_TRYTES_HASH];
  memcpy(hash_trytes, NULL_HASH, NUM_TRYTES_HASH);
  hash_trytes[0] = 'A';

  tryte_t bundle_trytes[NUM_TRYTES_BUNDLE];
  memcpy(bundle_trytes, NULL_HASH, NUM_TRYTES_BUNDLE);
  bundle_trytes[1] = 'A';

  for (size_t i = 0; i < 24; i++) {
    flex_trits_from_trytes(txs[i].consensus.hash, NUM_TRITS_HASH, hash_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    flex_trits_from_trytes(txs[i].essence.bundle, NUM_TRITS_BUNDLE, bundle_trytes, NUM_TRYTES_BUNDLE,
                           NUM_TRYTES_BUNDLE);
    TEST_ASSERT(iota_tangle_transaction_store(&tangle, &txs[i]) == RC_OK);
    hash243_queue_push(&req->bundles, txs[i].essence.bundle);
    hash_trytes[0]++;
    bundle_trytes[1] = 'A' + (i + 1) % 8;
  }

  TEST_ASSERT(iota_api_find_transactions(&api, &tangle, req, res, &error) == RC_API_MAX_FIND_TRANSACTIONS);
  TEST_ASSERT(error == NULL);

  find_transactions_req_free(&req);
  find_transactions_res_free(&res);
  error_res_free(&error);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;
  api.conf.max_find_transactions = 1024;

  RUN_TEST(test_find_transactions_bundles_only);
  RUN_TEST(test_find_transactions_addresses_only);
  RUN_TEST(test_find_transactions_tags_only);
  RUN_TEST(test_find_transactions_approvees_only);
  RUN_TEST(test_find_transactions_intersection);
  RUN_TEST(test_find_transactions_no_input);

  api.conf.max_find_transactions = 10;

  RUN_TEST(test_find_transactions_max);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
