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
      "\"" TEST_81_TRYTES_1
      "\"],\"approvees\":["
      "\"" TEST_81_TRYTES_2
      "\"],\"bundles\":["
      "\"" TEST_81_TRYTES_3
      "\"],\"tags\":["
      "\"" TEST_27_TRYTES_1 "\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trit_t tag[FLEX_TRIT_SIZE_81] = {};

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->addresses, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->approvees, hash) == RC_OK);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_3,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&find_tran->bundles, hash) == RC_OK);

  flex_trits_from_trytes(tag, NUM_TRITS_TAG, (const tryte_t*)TEST_27_TRYTES_1,
                         NUM_TRYTES_TAG, NUM_TRYTES_TAG);
  TEST_ASSERT(hash81_queue_push(&find_tran->tags, tag) == RC_OK);

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
      "[\"" TEST_81_TRYTES_1
      "\", "
      "\"" TEST_81_TRYTES_2
      "\", "
      "\"" TEST_81_TRYTES_3 "\"]}";

  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(
      &serializer, json_text, deserialize_find_tran);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash,
                           hash243_queue_at(&deserialize_find_tran->hashes, 0),
                           FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash,
                           hash243_queue_at(&deserialize_find_tran->hashes, 1),
                           FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_3,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash,
                           hash243_queue_at(&deserialize_find_tran->hashes, 2),
                           FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT(3, hash243_queue_count(&deserialize_find_tran->hashes));

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
      "{\"appName\":\"" TEST_INFO_APP_NAME
      "\",\"appVersion\":\"" TEST_INFO_APP_VERSION
      "\",\"duration\":1,"
      "\"jreAvailableProcessors\""
      ":" STR(TEST_INFO_JRE_AVAILABLE_PROCESSORS)
      ",\"jreFreeMemory\":" STR(TEST_INFO_JRE_FREE_MEMORY)
      ","
      "\"jreMaxMemory\":" STR(TEST_INFO_JRE_MAX_MEMORY)
      ",\"jreTotalMemory\":" STR(TEST_INFO_JRE_TOTAL_MEMORY)
      ","
      "\"latestMilestone\":"
      "\"" TEST_81_TRYTES_1
      "\","
      "\"latestMilestoneIndex\""
      ":" STR(TEST_INFO_LATEST_MILESTONE_INDEX)
      ","
      "\"latestSolidSubtangleMilestone\":"
      "\"" TEST_81_TRYTES_2
      "\","
      "\"latestSolidSubtangleMilestoneIndex\""
      ":" STR(TEST_INFO_LATEST_SS_MILESTONE_INDEX)
      ",\"neighbors\":" STR(TEST_INFO_NEIGHBORS)
      ",\"packetsQueueSize\":" STR(TEST_INFO_PACKETS_QUEUE_SIZE)
      ",\"time\":" STR(TEST_INFO_TIME)
      ",\"tips\":" STR(TEST_INFO_TIPS)
      ","
      "\"transactionsToRequest\""
      ":" STR(TEST_INFO_TRANSACTIONS_TO_REQUEST) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(&serializer, json_text,
                                                       node_info);

  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_NAME, node_info->app_name->data);
  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_VERSION, node_info->app_version->data);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_JRE_AVAILABLE_PROCESSORS,
                        node_info->jre_available_processors);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_FREE_MEMORY, node_info->jre_free_memory);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_MAX_MEMORY, node_info->jre_max_memory);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_TOTAL_MEMORY,
                         node_info->jre_total_memory);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, node_info->latest_milestone,
                           FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT8(TEST_INFO_LATEST_MILESTONE_INDEX,
                         node_info->latest_milestone_index);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, node_info->latest_solid_subtangle_milestone,
                           FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT8(TEST_INFO_LATEST_SS_MILESTONE_INDEX,
                         node_info->latest_solid_subtangle_milestone_index);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_NEIGHBORS, node_info->neighbors);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_PACKETS_QUEUE_SIZE,
                        node_info->packets_queue_size);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_TIME, node_info->time);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_TIPS, node_info->tips);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_TRANSACTIONS_TO_REQUEST,
                        node_info->trans_to_request);

  get_node_info_res_free(&node_info);
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
      "\"address\":\"" TEST_NEIGHBORS_NEIGHBOR1
      "\",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX1)
      ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX1)
      ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX1)
      "},{\"address\":\"" TEST_NEIGHBORS_NEIGHBOR2
      "\",\"numberOfAllTransactions\":" STR(TEST_NEIGHBORS_NUMALLTX2)
      ",\"numberOfInvalidTransactions\":" STR(TEST_NEIGHBORS_NUMINVALIDTX2)
      ",\"numberOfNewTransactions\":" STR(TEST_NEIGHBORS_NUMNEWTX2) "}]}";

  get_neighbors_res_t* nbors = get_neighbors_res_new();

  serializer.vtable.get_neighbors_deserialize_response(&serializer, json_text,
                                                       nbors);

  neighbor_info_t* nb = get_neighbors_res_neighbor_at(nbors, 0);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR1, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX1, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX1, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX1, nb->new_trans_num);
  nb = get_neighbors_res_neighbor_at(nbors, 1);
  TEST_ASSERT_EQUAL_STRING(TEST_NEIGHBORS_NEIGHBOR2, nb->address->data);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMALLTX2, nb->all_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMINVALIDTX2, nb->invalid_trans_num);
  TEST_ASSERT_EQUAL_INT(TEST_NEIGHBORS_NUMNEWTX2, nb->new_trans_num);
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
  add_neighbors_req_free(&req);
}

void test_deserialize_add_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"addedNeighbors\":" STR(ADD_NEIGHBORS_RES) ",\"duration\":2}";

  add_neighbors_res_t* res = add_neighbors_res_new();

  serializer.vtable.add_neighbors_deserialize_response(&serializer, json_text,
                                                       res);

  TEST_ASSERT_EQUAL_INT(ADD_NEIGHBORS_RES, res->added_neighbors);

  add_neighbors_res_free(&res);
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
  const char* json_text =
      "{\"removedNeighbors\":" STR(REMOVE_NEIGHBORS_RES) ",\"duration\":2}";

  remove_neighbors_res_t res = 0;

  serializer.vtable.remove_neighbors_deserialize_response(&serializer,
                                                          json_text, &res);

  TEST_ASSERT_EQUAL_INT(REMOVE_NEIGHBORS_RES, res);
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
      "\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2
      "\","
      "\"" TEST_81_TRYTES_3 "\"],\"duration\":4}";

  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  get_tips_res_t* tips_res = get_tips_res_new();

  serializer.vtable.get_tips_deserialize_response(&serializer, json_text,
                                                  tips_res);

  hash243_stack_entry_t* hashes = tips_res->hashes;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_3,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hashes->hash, hash, FLEX_TRIT_SIZE_243);
  hashes = hashes->next;

  TEST_ASSERT_NULL(hashes);

  get_tips_res_free(&tips_res);
}

void test_serialize_get_trytes(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" TEST_81_TRYTES_1 "\"]}";
  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  TEST_ASSERT(flex_trits_from_trytes(hash, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->hashes, hash) == RC_OK);

  serializer.vtable.get_trytes_serialize_request(&serializer, req,
                                                 serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_trytes_req_free(&req);
}

void test_deserialize_get_trytes(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};

  flex_trit_t* tmp_trytes = NULL;
  get_trytes_res_t* res = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(&serializer, json_text,
                                                    res);
  tmp_trytes = hash8019_queue_at(&res->trytes, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (const tryte_t*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(tmp_trytes, hash, FLEX_TRIT_SIZE_8019);
  tmp_trytes = hash8019_queue_at(&res->trytes, 1);
  TEST_ASSERT_NULL(tmp_trytes);

  get_trytes_res_free(&res);
}

void test_serialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" TEST_81_TRYTES_1 "\"],\"tips\":[\"" TEST_81_TRYTES_2 "\"]}";

  get_inclusion_state_req_t* get_is = get_inclusion_state_req_new();
  char_buffer_t* serializer_out = char_buffer_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->hashes, trits_243) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&get_is->tips, trits_243) == RC_OK);

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
      "\"" TEST_81_TRYTES_1
      "\"]"
      ",\"threshold\":" STR(TEST_BALANCES_SERIALIZE_THRESHOLD) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_req_t* get_bal = get_balances_req_new();

  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(get_bal);
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT(hash243_queue_push(&get_bal->addresses, hash) == RC_OK);

  get_bal->threshold = TEST_BALANCES_SERIALIZE_THRESHOLD;
  serializer.vtable.get_balances_serialize_request(&serializer, get_bal,
                                                   serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_balances_req_free(&get_bal);
}

void test_deserialize_get_balances(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"balances\": "
      "[\"" STR(TEST_BALANCES_BALANCE)
      "\"], "
      "\"references\": "
      "[\"" TEST_81_TRYTES_1
      "\"], "
      "\"milestoneIndex\":" STR(TEST_BALANCES_MILESTONEINDEX) "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  get_balances_res_t* deserialize_get_bal = get_balances_res_new();
  serializer.vtable.get_balances_deserialize_response(&serializer, json_text,
                                                      deserialize_get_bal);

  TEST_ASSERT_EQUAL_UINT64(TEST_BALANCES_BALANCE, get_balances_res_balances_at(
                                                      deserialize_get_bal, 0));

  TEST_ASSERT_EQUAL_MEMORY(hash,
                           hash243_queue_at(&deserialize_get_bal->milestone, 0),
                           FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_INT(TEST_BALANCES_MILESTONEINDEX,
                        deserialize_get_bal->milestoneIndex);

  get_balances_res_free(&deserialize_get_bal);
}

void test_serialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(
          TEST_TRANSACTION_TO_APPROVE_DEPTH) ",\"reference\":"
                                             "\"" TEST_81_TRYTES_1 "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_tx_approve =
      get_transactions_to_approve_req_new();

  TEST_ASSERT(flex_trits_from_trytes(get_tx_approve->reference, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));

  get_tx_approve->depth = TEST_TRANSACTION_TO_APPROVE_DEPTH;
  serializer.vtable.get_transactions_to_approve_serialize_request(
      &serializer, get_tx_approve, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  get_transactions_to_approve_req_free(&get_tx_approve);
}

void test_deserialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"trunkTransaction\": "
      "\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":"
      "\"" TEST_81_TRYTES_2 "\"}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};

  get_transactions_to_approve_res_t* deserialize_get_tx_approve =
      get_transactions_to_approve_res_new();
  serializer.vtable.get_transactions_to_approve_deserialize_response(
      &serializer, json_text, deserialize_get_tx_approve);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_1,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, deserialize_get_tx_approve->trunk,
                           FLEX_TRIT_SIZE_243);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (const tryte_t*)TEST_81_TRYTES_2,
                         NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, deserialize_get_tx_approve->branch,
                           FLEX_TRIT_SIZE_243);

  get_transactions_to_approve_res_free(&deserialize_get_tx_approve);
}

void test_serialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_81_TRYTES_1
      "\",\"branchTransaction\":\"" TEST_81_TRYTES_2
      "\",\"minWeightMagnitude\":" STR(
          TEST_MWM) ",\"trytes\":[\"" TEST_2673_TRYTES_1
                    "\",\"" TEST_2673_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();
  flex_trit_t trits_8019[FLEX_TRIT_SIZE_8019];

  TEST_ASSERT(flex_trits_from_trytes(attach_req->trunk, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(flex_trits_from_trytes(attach_req->branch, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));

  TEST_ASSERT(flex_trits_from_trytes(
      trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_1, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION));
  TEST_ASSERT(hash8019_queue_push(&attach_req->trytes, trits_8019) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(
      trits_8019, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_2, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION));
  TEST_ASSERT(hash8019_queue_push(&attach_req->trytes, trits_8019) == RC_OK);

  attach_req->mwm = TEST_MWM;

  serializer.vtable.attach_to_tangle_serialize_request(&serializer, attach_req,
                                                       serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  attach_to_tangle_req_free(&attach_req);
}

void test_deserialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text = "{\"trytes\":[\"" TEST_2673_TRYTES_1
                          "\",\"" TEST_2673_TRYTES_2 "\"],\"duration\":4}";
  flex_trit_t* tmp_hash = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};

  attach_to_tangle_res_t* attach_res = attach_to_tangle_res_new();

  serializer.vtable.attach_to_tangle_deserialize_response(
      &serializer, json_text, attach_res);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 0);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (const tryte_t*)TEST_2673_TRYTES_1,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 1);
  flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION,
                         (const tryte_t*)TEST_2673_TRYTES_2,
                         NUM_TRYTES_SERIALIZED_TRANSACTION,
                         NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT_EQUAL_MEMORY(hash, tmp_hash, FLEX_TRIT_SIZE_8019);

  tmp_hash = attach_to_tangle_res_trytes_at(attach_res, 3);
  TEST_ASSERT_NULL(tmp_hash);

  attach_to_tangle_res_free(&attach_res);
}

void test_serialize_broadcast_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"broadcastTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYTES_3 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  broadcast_transactions_req_t* req = broadcast_transactions_req_new();
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_3, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT(len);

  TEST_ASSERT(hash8019_stack_push(&req->trytes, tx_trits) == RC_OK);

  serializer.vtable.broadcast_transactions_serialize_request(&serializer, req,
                                                             serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  broadcast_transactions_req_free(&req);
}

void test_serialize_store_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"storeTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYTES_1 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  store_transactions_req_t* req = store_transactions_req_new();
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];
  size_t len = flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
      (const tryte_t*)TEST_2673_TRYTES_1, NUM_TRYTES_SERIALIZED_TRANSACTION,
      NUM_TRYTES_SERIALIZED_TRANSACTION);
  TEST_ASSERT(len);

  TEST_ASSERT(hash8019_stack_push(&req->trytes, tx_trits) == RC_OK);

  serializer.vtable.store_transactions_serialize_request(&serializer, req,
                                                         serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  store_transactions_req_free(&req);
}

void test_serialize_check_consistency(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"checkConsistency\",\"tails\":"
      "[\"" TEST_81_TRYTES_1
      "\","
      "\"" TEST_81_TRYTES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  check_consistency_req_t* req = check_consistency_req_new();
  flex_trit_t trits_243[FLEX_TRIT_SIZE_243];

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_1,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->hashes, trits_243) == RC_OK);

  TEST_ASSERT(flex_trits_from_trytes(trits_243, NUM_TRITS_HASH,
                                     (const tryte_t*)TEST_81_TRYTES_2,
                                     NUM_TRYTES_HASH, NUM_TRYTES_HASH));
  TEST_ASSERT(hash243_queue_push(&req->hashes, trits_243) == RC_OK);

  serializer.vtable.check_consistency_serialize_request(&serializer, req,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  check_consistency_req_free(&req);
}

void test_deserialize_check_consistency(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* res_true = "{\"state\":true,\"info\":\" \",\"duration\":0}";
  const char* res_false =
      "{\"state\":false,\"info\":\"" CONSISTENCY_INFO "\",\"duration\":0}";

  check_consistency_res_t* consistent_res = check_consistency_res_new();
  serializer.vtable.check_consistency_deserialize_response(
      &serializer, res_true, consistent_res);
  TEST_ASSERT_TRUE(consistent_res->state == true);
  TEST_ASSERT_EQUAL_STRING(" ", consistent_res->info->data);
  check_consistency_res_free(consistent_res);

  consistent_res = check_consistency_res_new();
  serializer.vtable.check_consistency_deserialize_response(
      &serializer, res_false, consistent_res);
  TEST_ASSERT_TRUE(consistent_res->state == false);
  TEST_ASSERT_EQUAL_STRING(CONSISTENCY_INFO, consistent_res->info->data);
  check_consistency_res_free(consistent_res);
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

  RUN_TEST(test_serialize_check_consistency);
  RUN_TEST(test_deserialize_check_consistency);
  return UNITY_END();
}
