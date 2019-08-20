/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshots_provider.h"
#include "ciri/consensus/test_utils/bundle.h"
#include "ciri/consensus/test_utils/tangle.h"
#include "ciri/node/node.h"

static char *tangle_test_db_path = "ciri/api/tests/tangle-test.db";
static char *tangle_db_path = "common/storage/tangle.db";
static connection_config_t config;
static iota_api_t api;
static tangle_t tangle;
static core_t core;

void setUp(void) { TEST_ASSERT(tangle_setup(&tangle, &config, tangle_test_db_path, tangle_db_path) == RC_OK); }

void tearDown(void) { TEST_ASSERT(tangle_cleanup(&tangle, tangle_test_db_path) == RC_OK); }

void test_broadcast_transactions_empty(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  error_res_t *error = NULL;

  TEST_ASSERT(iota_api_broadcast_transactions(&api, req, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(broadcaster_stage_size(&api.core->node.broadcaster), 0);

  broadcast_transactions_req_free(&req);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
}

void test_broadcast_transactions(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  error_res_t *error = NULL;
  tryte_t const *const txs_trytes[4] = {TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
                                        TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  byte_t bytes[GOSSIP_MAX_BYTES_LENGTH];
  protocol_gossip_queue_entry_t *entry = NULL;

  memset(bytes, 0, GOSSIP_MAX_BYTES_LENGTH);

  // Broadcasting 4 transactions

  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, txs_trytes[i], NUM_TRYTES_SERIALIZED_TRANSACTION,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
    broadcast_transactions_req_trytes_add(req, tx_trits);
  }
  TEST_ASSERT(iota_api_broadcast_transactions(&api, req, &error) == RC_OK);
  TEST_ASSERT(error == NULL);

  TEST_ASSERT_EQUAL_INT(processor_stage_size(&api.core->node.processor), 4);

  for (size_t i = 0; i < 4; i++) {
    flex_trits_to_bytes(bytes, NUM_TRITS_SERIALIZED_TRANSACTION, broadcat_transactions_req_trytes_get(req, i),
                        NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
    entry = protocol_gossip_queue_pop(&api.core->node.processor.queue);
    TEST_ASSERT_EQUAL_MEMORY(entry->packet.content, bytes, GOSSIP_MAX_BYTES_LENGTH);
    free(entry);
  }

  broadcast_transactions_req_free(&req);
  error_res_free(&error);
  TEST_ASSERT(req == NULL);
}

int main(void) {
  UNITY_BEGIN();
  TEST_ASSERT(storage_init() == RC_OK);

  config.db_path = tangle_test_db_path;
  api.core = &core;
  broadcaster_stage_init(&api.core->node.broadcaster, &api.core->node);
  TEST_ASSERT(requester_init(&api.core->node.transaction_requester, &api.core->node) == RC_OK);
  TEST_ASSERT(iota_consensus_conf_init(&api.core->consensus.conf) == RC_OK);
  api.core->consensus.conf.snapshot_timestamp_sec = 1536845195;
  api.core->consensus.conf.mwm = 1;

  // Avoid complete initialization with state file loading
  iota_snapshot_reset(&api.core->consensus.snapshots_provider.initial_snapshot, &api.core->consensus.conf);
  iota_snapshot_reset(&api.core->consensus.snapshots_provider.latest_snapshot, &api.core->consensus.conf);

  iota_consensus_transaction_validator_init(&api.core->consensus.transaction_validator,
                                            &api.core->consensus.snapshots_provider,
                                            &api.core->node.transaction_requester, &api.core->consensus.conf);

  RUN_TEST(test_broadcast_transactions_empty);
  RUN_TEST(test_broadcast_transactions);

  TEST_ASSERT(storage_destroy() == RC_OK);
  return UNITY_END();
}
