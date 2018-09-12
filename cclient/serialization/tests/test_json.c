/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "test_json.h"

void test_serialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"findTransactions\",\"addresses\":["
      "\"" FIND_TRANSACTIONS_SERIALIZE_ADDRESS
      "\"],\"approvees\":["
      "\"" FIND_TRANSACTIONS_SERIALIZE_APPROVEE
      "\"],\"bundles\":["
      "\"" FIND_TRANSACTIONS_SERIALIZE_BUNDLE
      "\"],\"tags\":[\"" FIND_TRANSACTIONS_SERIALIZE_TAG "\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  find_transactions_req_add_tag(find_tran, FIND_TRANSACTIONS_SERIALIZE_TAG);
  find_transactions_req_add_approvee(find_tran,
                                     FIND_TRANSACTIONS_SERIALIZE_APPROVEE);
  find_transactions_req_add_address(find_tran,
                                    FIND_TRANSACTIONS_SERIALIZE_ADDRESS);
  find_transactions_req_add_bundle(find_tran,
                                   FIND_TRANSACTIONS_SERIALIZE_BUNDLE);

  serializer.vtable.find_transactions_serialize_request(&serializer, find_tran,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  find_transactions_req_free(&find_tran);
}

void test_deserialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\": "
      "[\"" TEST_HASH1
      "\", "
      "\"" TEST_HASH2
      "\", "
      "\"" TEST_HASH3 "\"]}";

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(
      &serializer, json_text, deserialize_find_tran);
  TEST_ASSERT_EQUAL_STRING(
      TEST_HASH1, find_transactions_res_hash_at(deserialize_find_tran, 0));
  TEST_ASSERT_EQUAL_STRING(
      TEST_HASH2, find_transactions_res_hash_at(deserialize_find_tran, 1));
  TEST_ASSERT_EQUAL_STRING(
      NULL, find_transactions_res_hash_at(deserialize_find_tran, 3));
  find_transactions_res_free(&deserialize_find_tran);
}

void test_serialize_get_node_info(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNodeInfo\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_node_info_serialize_request(&serializer,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_node_info(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"appName\":\"" GET_NODE_INFO_DESERIALIZE_APP_NAME
      "\",\"appVersion\":\"" GET_NODE_INFO_DESERIALIZE_APP_VERSION
      "\",\"duration\":1,"
      "\"jreAvailableProcessors\""
      ":" STR(GET_NODE_INFO_DESERIALIZE_JRE_AVAILABLE_PROCESSORS)
      ",\"jreFreeMemory\":" STR(GET_NODE_INFO_DESERIALIZE_JRE_FREE_MEMORY)
      ","
      "\"jreMaxMemory\":" STR(GET_NODE_INFO_DESERIALIZE_JRE_MAX_MEMORY)
      ",\"jreTotalMemory\":" STR(GET_NODE_INFO_DESERIALIZE_JRE_TOTAL_MEMORY)
      ","
      "\"latestMilestone\":"
      "\"" GET_NODE_INFO_DESERIALIZE_LATEST_MILESTONE
      "\","
      "\"latestMilestoneIndex\""
      ":" STR(GET_NODE_INFO_DESERIALIZE_LATEST_MILESTONE_INDEX)
      ","
      "\"latestSolidSubtangleMilestone\":"
      "\"" GET_NODE_INFO_DESERIALIZE_LATEST_SS_MILESTONE
      "\","
      "\"latestSolidSubtangleMilestoneIndex\""
      ":" STR(GET_NODE_INFO_DESERIALIZE_LATEST_SS_MILESTONE_INDEX)
      ",\"neighbors\":" STR(GET_NODE_INFO_DESERIALIZE_NEIGHBORS)
      ",\"packetsQueueSize\":" STR(GET_NODE_INFO_DESERIALIZE_PACKETS_QUEUE_SIZE)
      ",\"time\":" STR(GET_NODE_INFO_DESERIALIZE_TIME)
      ",\"tips\":" STR(GET_NODE_INFO_DESERIALIZE_TIPS)
      ","
      "\"transactionsToRequest\""
      ":" STR(GET_NODE_INFO_DESERIALIZE_TRANSACTIONS_TO_REQUEST) "}";

  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(&serializer, json_text,
                                                       node_info);

  TEST_ASSERT_EQUAL_STRING(GET_NODE_INFO_DESERIALIZE_APP_NAME,
                           node_info->app_name->data);
  TEST_ASSERT_EQUAL_STRING(GET_NODE_INFO_DESERIALIZE_APP_VERSION,
                           node_info->app_version->data);
  TEST_ASSERT_EQUAL_INT(GET_NODE_INFO_DESERIALIZE_JRE_AVAILABLE_PROCESSORS,
                        node_info->jre_available_processors);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_JRE_FREE_MEMORY,
                         node_info->jre_free_memory);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_JRE_MAX_MEMORY,
                         node_info->jre_max_memory);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_JRE_TOTAL_MEMORY,
                         node_info->jre_total_memory);
  TEST_ASSERT_EQUAL_STRING(GET_NODE_INFO_DESERIALIZE_LATEST_MILESTONE,
                           node_info->latest_milestone->data);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_LATEST_MILESTONE_INDEX,
                         node_info->latest_milestone_index);
  TEST_ASSERT_EQUAL_STRING(GET_NODE_INFO_DESERIALIZE_LATEST_SS_MILESTONE,
                           node_info->latest_solid_subtangle_milestone->data);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_LATEST_SS_MILESTONE_INDEX,
                         node_info->latest_solid_subtangle_milestone_index);
  TEST_ASSERT_EQUAL_INT(GET_NODE_INFO_DESERIALIZE_NEIGHBORS,
                        node_info->neighbors);
  TEST_ASSERT_EQUAL_INT(GET_NODE_INFO_DESERIALIZE_PACKETS_QUEUE_SIZE,
                        node_info->packets_queue_size);
  TEST_ASSERT_EQUAL_INT8(GET_NODE_INFO_DESERIALIZE_TIME, node_info->time);
  TEST_ASSERT_EQUAL_INT(GET_NODE_INFO_DESERIALIZE_TIPS, node_info->tips);
  TEST_ASSERT_EQUAL_INT(GET_NODE_INFO_DESERIALIZE_TRANSACTIONS_TO_REQUEST,
                        node_info->trans_to_request);

  get_node_info_res_free(node_info);
}

void test_serialize_get_neighbors(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNeighbors\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_neighbors_serialize_request(&serializer,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"duration\":37,\"neighbors\":[{"
      "\"address\":\"" GET_NEIGHBORS_DESERIALIZE_NEIGHBOR1
      "\",\"numberOfAllTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMALLTX1)
      ",\"numberOfInvalidTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMINVALIDTX1)
      ",\"numberOfNewTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMNEWTX1)
      "},{\"address\":\"" GET_NEIGHBORS_DESERIALIZE_NEIGHBOR2
      "\",\"numberOfAllTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMALLTX2)
      ",\"numberOfInvalidTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMINVALIDTX2)
      ",\"numberOfNewTransactions\":" STR(GET_NEIGHBORS_DESERIALIZE_NUMNEWTX2) "}]}";

  get_neighbors_res_t* nbors = get_neighbors_res_new();

  serializer.vtable.get_neighbors_deserialize_response(&serializer, json_text,
                                                       nbors);

  neighbor_info_t* nb = get_neighbors_res_neighbor_at(nbors, 0);
  TEST_ASSERT_EQUAL_STRING(GET_NEIGHBORS_DESERIALIZE_NEIGHBOR1,
                           nb->address->data);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMALLTX1, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMINVALIDTX1,
                        nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMNEWTX1, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 1);
  TEST_ASSERT_EQUAL_STRING(GET_NEIGHBORS_DESERIALIZE_NEIGHBOR2,
                           nb->address->data);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMALLTX2, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMINVALIDTX2,
                        nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(GET_NEIGHBORS_DESERIALIZE_NUMNEWTX2, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 2);
  TEST_ASSERT_NULL(nb);
  get_neighbors_res_free(nbors);
}

void test_serialize_add_neighbors(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"addNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1
      "\",\"" TEST_NEIGHBOR2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  add_neighbors_req_t* req = add_neighbors_req_new();
  add_neighbors_req_add(req, TEST_NEIGHBOR1);
  add_neighbors_req_add(req, TEST_NEIGHBOR2);

  serializer.vtable.add_neighbors_serialize_request(&serializer, req,
                                                    serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  add_neighbors_req_free(req);
}

void test_deserialize_add_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"addedNeighbors\":" STR(
      ADD_NEIGHBORS_DESERIALIZE_RES) ",\"duration\":2}";

  add_neighbors_res_t res = 0;

  serializer.vtable.add_neighbors_deserialize_response(&serializer, json_text,
                                                       &res);

  TEST_ASSERT_EQUAL_INT(ADD_NEIGHBORS_DESERIALIZE_RES, res);
}

void test_serialize_remove_neighbors(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"removeNeighbors\",\"uris\":[\"" TEST_NEIGHBOR1
      "\",\"" TEST_NEIGHBOR2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  remove_neighbors_req_t* req = remove_neighbors_req_new();
  remove_neighbors_req_add(req, TEST_NEIGHBOR1);
  remove_neighbors_req_add(req, TEST_NEIGHBOR2);

  serializer.vtable.remove_neighbors_serialize_request(&serializer, req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  remove_neighbors_req_free(req);
}

void test_deserialize_remove_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"removedNeighbors\":" STR(
      REMOVE_NEIGHBORS_DESERIALIZE_RES) ",\"duration\":2}";

  remove_neighbors_res_t res = 0;

  serializer.vtable.remove_neighbors_deserialize_response(&serializer,
                                                          json_text, &res);

  TEST_ASSERT_EQUAL_INT(REMOVE_NEIGHBORS_DESERIALIZE_RES, res);
}

void test_serialize_get_tips(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getTips\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_tips_serialize_request(&serializer, serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

void test_deserialize_get_tips(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\":["
      "\"" TEST_HASH1
      "\","
      "\"" TEST_HASH2
      "\","
      "\"" TEST_HASH3 "\"],\"duration\":4}";

  get_tips_res_t* tips = get_tips_res_new();

  serializer.vtable.get_tips_deserialize_response(&serializer, json_text, tips);
  TEST_ASSERT_EQUAL_STRING(TEST_HASH1, get_tips_res_hash_at(tips, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_HASH2, get_tips_res_hash_at(tips, 1));
  TEST_ASSERT_EQUAL_STRING(NULL, get_tips_res_hash_at(tips, 3));
  get_tips_res_free(tips);
}

void test_serialize_get_trytes(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" GET_TRYTES_SERIALIZE_HASH "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  get_trytes_req_add(req, GET_TRYTES_SERIALIZE_HASH);

  serializer.vtable.get_trytes_serialize_request(&serializer, req,
                                                 serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_req_free(req);
}

void test_deserialize_get_trytes(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":["
      "\"" GET_TRYTES_DESERIALIZE_TX "\"]}";

  get_trytes_res_t* tips = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(&serializer, json_text,
                                                    tips);
  TEST_ASSERT_EQUAL_STRING(GET_TRYTES_DESERIALIZE_TX,
                           get_trytes_res_at(tips, 0));
  TEST_ASSERT_EQUAL_STRING(NULL, get_trytes_res_at(tips, 3));
  get_trytes_res_free(tips);
}

void test_serialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" INCLUSION_STATES_HASH "\"],\"tips\":[\"" INCLUSION_STATES_TIPS
      "\"]}";

  get_inclusion_state_req_t* get_is = get_inclusion_state_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  get_inclusion_state_req_add_hash(get_is, INCLUSION_STATES_HASH);
  get_inclusion_state_req_add_tip(get_is, INCLUSION_STATES_TIPS);

  serializer.vtable.get_inclusion_state_serialize_request(&serializer, get_is,
                                                          serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_inclusion_state_req_free(&get_is);
}

void test_deserialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"states\": [true,false]}";
  get_inclusion_state_res_t* deserialize_get_is = get_inclusion_state_res_new();

  serializer.vtable.get_inclusion_state_deserialize_response(
      &serializer, json_text, deserialize_get_is);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 0) ==
                   true);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 1) ==
                   false);
  TEST_ASSERT_TRUE(get_inclusion_state_res_bool_at(deserialize_get_is, 2) ==
                   false);
  TEST_ASSERT_FALSE(get_inclusion_state_res_bool_at(deserialize_get_is, 2) ==
                    true);
  get_inclusion_state_res_free(&deserialize_get_is);
}

void test_serialize_get_balances(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getBalances\",\"addresses\":["
      "\"" GET_BALANCES_SERIALIZE_ADDRESS
      "\"]"
      ",\"threshold\":" STR(GET_BALANCES_SERIALIZE_THRESHOLD) "}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_req_t* get_bal = get_balances_req_new();
  get_balances_req_add_address(get_bal, GET_BALANCES_SERIALIZE_ADDRESS);
  get_bal->threshold = GET_BALANCES_SERIALIZE_THRESHOLD;
  serializer.vtable.get_balances_serialize_request(&serializer, get_bal,
                                                   serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_balances_req_free(get_bal);
}

void test_deserialize_get_balances(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"balances\": "
      "[\"" GET_BALANCES_DESERIALIZE_BALANCE
      "\"], "
      "\"references\": "
      "[\"" GET_BALANCES_DESERIALIZE_REFERENCE
      "\"], "
      "\"milestoneIndex\":" STR(GET_BALANCES_DESERIALIZE_MILESTONEINDEX) "}";

  get_balances_res_t* deserialize_get_bal = get_balances_res_new();
  serializer.vtable.get_balances_deserialize_response(&serializer, json_text,
                                                      deserialize_get_bal);

  TEST_ASSERT_EQUAL_STRING(
      GET_BALANCES_DESERIALIZE_BALANCE,
      get_balances_res_balances_at(deserialize_get_bal, 0));
  TEST_ASSERT_EQUAL_STRING(
      GET_BALANCES_DESERIALIZE_REFERENCE,
      get_balances_res_milestone_at(deserialize_get_bal, 0));
  TEST_ASSERT_EQUAL_INT(GET_BALANCES_DESERIALIZE_MILESTONEINDEX,
                        deserialize_get_bal->milestoneIndex);
}

void test_serialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(GET_TRANSACTION_TO_APPROVE_DEPTH)
      ",\"reference\":\"" GET_TRANSACTION_TO_APPROVE_HASH "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_txn_approve =
      get_transactions_to_approve_req_new();
  get_transactions_to_approve_req_set_reference(
      get_txn_approve, GET_TRANSACTION_TO_APPROVE_HASH);
  get_txn_approve->depth = GET_TRANSACTION_TO_APPROVE_DEPTH;
  serializer.vtable.get_transactions_to_approve_serialize_request(
      &serializer, get_txn_approve, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_req_free(&get_txn_approve);
}

void test_deserialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trunkTransaction\": "
      "\"" GET_TRANSACTION_TO_APPROVE_HASH
      "\",\"branchTransaction\":"
      "\"" GET_TRANSACTION_TO_APPROVE_HASH "\"}";

  get_transactions_to_approve_res_t* deserialize_get_txn_approve =
      get_transactions_to_approve_res_new();
  serializer.vtable.get_transactions_to_approve_deserialize_response(
      &serializer, json_text, deserialize_get_txn_approve);

  TEST_ASSERT_EQUAL_STRING(GET_TRANSACTION_TO_APPROVE_HASH,
                           deserialize_get_txn_approve->trunk->data);
  TEST_ASSERT_EQUAL_STRING(GET_TRANSACTION_TO_APPROVE_HASH,
                           deserialize_get_txn_approve->branch->data);

  get_transactions_to_approve_res_free(&deserialize_get_txn_approve);
}

void test_serialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_HASH1
      "\",\"branchTransaction\":\"" TEST_HASH2 "\",\"minWeightMagnitude\":" STR(
          TEST_MWM) ",\"trytes\":[\"" TEST_RAW_TRYTES1 "\",\"" TEST_RAW_TRYTES2
                    "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();

  attach_to_tangle_req_set_trunk(attach_req, TEST_HASH1);
  attach_to_tangle_req_set_branch(attach_req, TEST_HASH2);
  attach_to_tangle_req_add_trytes(attach_req, TEST_RAW_TRYTES1);
  attach_to_tangle_req_add_trytes(attach_req, TEST_RAW_TRYTES2);
  attach_to_tangle_req_set_mwm(attach_req, TEST_MWM);
  serializer.vtable.attach_to_tangle_serialize_request(&serializer, attach_req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  attach_to_tangle_req_free(&attach_req);
}

void test_deserialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":[\"" TEST_HASH1 "\",\"" TEST_HASH2 "\"],\"duration\":4}";

  attach_to_tangle_res_t* trytes = attach_to_tangle_res_new();

  serializer.vtable.attach_to_tangle_deserialize_response(&serializer,
                                                          json_text, trytes);
  TEST_ASSERT_EQUAL_STRING(TEST_HASH1,
                           attach_to_tangle_res_trytes_at(trytes, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_HASH2,
                           attach_to_tangle_res_trytes_at(trytes, 1));
  TEST_ASSERT_EQUAL_STRING(NULL, attach_to_tangle_res_trytes_at(trytes, 3));
  attach_to_tangle_res_free(trytes);
}

void test_serialize_broadcast_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"broadcastTransactions\",\"trytes\":["
      "\"" BROADCAST_TX_TRYTES "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  broadcast_transactions_req_t* req = broadcast_transactions_req_new();
  broadcast_transactions_req_add(req, BROADCAST_TX_TRYTES);

  serializer.vtable.broadcast_transactions_serialize_request(&serializer, req,
                                                             serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  broadcast_transactions_req_free(req);
}

void test_serialize_store_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"storeTransactions\",\"trytes\":["
      "\"" STORE_TX_TRYTES "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  store_transactions_req_t* req = store_transactions_req_new();
  store_transactions_req_add(req, STORE_TX_TRYTES);

  serializer.vtable.store_transactions_serialize_request(&serializer, req,
                                                         serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  store_transactions_req_free(req);
}

void test_serialize_were_addresses_spent_from(void) {
  // TODO
}

void test_deserialize_were_addresses_spent_from(void) {
  // TODO
}

int main(void) {
  UNITY_BEGIN();

  // find_transactions
  RUN_TEST(test_serialize_find_transactions);
  RUN_TEST(test_deserialize_find_transactions);

  // get_node_info
  RUN_TEST(test_serialize_get_node_info);
  RUN_TEST(test_deserialize_get_node_info);

  // get_neighbors
  RUN_TEST(test_serialize_get_neighbors);
  RUN_TEST(test_deserialize_get_neighbors);

  // add_neighbors
  RUN_TEST(test_serialize_add_neighbors);
  RUN_TEST(test_deserialize_add_neighbors);

  // remove_neighbors
  RUN_TEST(test_serialize_remove_neighbors);
  RUN_TEST(test_deserialize_remove_neighbors);

  RUN_TEST(test_serialize_get_tips);
  RUN_TEST(test_deserialize_get_tips);

  RUN_TEST(test_serialize_get_trytes);
  RUN_TEST(test_deserialize_get_trytes);

  RUN_TEST(test_serialize_get_inclusion_states);
  RUN_TEST(test_deserialize_get_inclusion_states);

  RUN_TEST(test_serialize_get_balances);
  RUN_TEST(test_deserialize_get_balances);

  RUN_TEST(test_serialize_get_transactions_to_approve);
  RUN_TEST(test_deserialize_get_transactions_to_approve);

  RUN_TEST(test_serialize_attach_to_tangle);
  RUN_TEST(test_deserialize_attach_to_tangle);

  RUN_TEST(test_serialize_broadcast_transactions);

  RUN_TEST(test_serialize_store_transactions);

  RUN_TEST(test_serialize_were_addresses_spent_from);
  RUN_TEST(test_deserialize_were_addresses_spent_from);
  return UNITY_END();
}
