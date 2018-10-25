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
      "\"" TEST_81_TRYRES_1
      "\"],\"approvees\":["
      "\"" TEST_81_TRYRES_2
      "\"],\"bundles\":["
      "\"" TEST_81_TRYRES_3
      "\"],\"tags\":["
      "\"" TEST_27_TRYRES_1 "\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  find_tran = find_transactions_req_add_tag(find_tran, TEST_27_TRYRES_1);
  find_tran = find_transactions_req_add_approvee(find_tran, TEST_81_TRYRES_2);
  find_tran = find_transactions_req_add_address(find_tran, TEST_81_TRYRES_1);
  find_tran = find_transactions_req_add_bundle(find_tran, TEST_81_TRYRES_3);

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
      "[\"" TEST_81_TRYRES_1
      "\", "
      "\"" TEST_81_TRYRES_2
      "\", "
      "\"" TEST_81_TRYRES_3 "\"]}";

  trit_array_p tmp_hash = NULL;
  trit_array_p hash1 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  trit_array_p hash2 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_2);
  trit_array_p hash3 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_3);

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(
      &serializer, json_text, deserialize_find_tran);

  tmp_hash = find_transactions_res_hash_at(deserialize_find_tran, 0);
  TEST_ASSERT_EQUAL_MEMORY(hash1->trits, tmp_hash->trits, tmp_hash->num_bytes);

  tmp_hash = find_transactions_res_hash_at(deserialize_find_tran, 1);
  TEST_ASSERT_EQUAL_MEMORY(hash2->trits, tmp_hash->trits, tmp_hash->num_bytes);

  tmp_hash = find_transactions_res_hash_at(deserialize_find_tran, 2);
  TEST_ASSERT_EQUAL_MEMORY(hash3->trits, tmp_hash->trits, tmp_hash->num_bytes);

  tmp_hash = find_transactions_res_hash_at(deserialize_find_tran, 3);
  TEST_ASSERT_NULL(tmp_hash);

  find_transactions_res_free(&deserialize_find_tran);
  trit_array_free(hash1);
  trit_array_free(hash2);
  trit_array_free(hash3);
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
      "\"" TEST_81_TRYRES_1
      "\","
      "\"latestMilestoneIndex\""
      ":" STR(TEST_INFO_LATEST_MILESTONE_INDEX)
      ","
      "\"latestSolidSubtangleMilestone\":"
      "\"" TEST_81_TRYRES_2
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

  trit_array_p last_m = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  trit_array_p last_sm = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_2);
  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(&serializer, json_text,
                                                       &node_info);

  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_NAME, node_info->app_name->data);
  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_VERSION, node_info->app_version->data);
  TEST_ASSERT_EQUAL_INT(TEST_INFO_JRE_AVAILABLE_PROCESSORS,
                        node_info->jre_available_processors);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_FREE_MEMORY, node_info->jre_free_memory);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_MAX_MEMORY, node_info->jre_max_memory);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_JRE_TOTAL_MEMORY,
                         node_info->jre_total_memory);
  TEST_ASSERT_EQUAL_MEMORY(last_m->trits, node_info->latest_milestone->trits,
                           last_m->num_bytes);
  TEST_ASSERT_EQUAL_INT8(TEST_INFO_LATEST_MILESTONE_INDEX,
                         node_info->latest_milestone_index);
  TEST_ASSERT_EQUAL_MEMORY(last_sm->trits,
                           node_info->latest_solid_subtangle_milestone->trits,
                           last_sm->num_bytes);
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

  trit_array_free(last_m);
  trit_array_free(last_sm);
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
  add_neighbors_req_free(req);
}

void test_deserialize_add_neighbors(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"addedNeighbors\":" STR(ADD_NEIGHBORS_RES) ",\"duration\":2}";

  add_neighbors_res_t res = 0;

  serializer.vtable.add_neighbors_deserialize_response(&serializer, json_text,
                                                       &res);

  TEST_ASSERT_EQUAL_INT(ADD_NEIGHBORS_RES, res);
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
      "\"" TEST_81_TRYRES_1
      "\","
      "\"" TEST_81_TRYRES_2
      "\","
      "\"" TEST_81_TRYRES_3 "\"],\"duration\":4}";

  trit_array_p tmp_tip = NULL;
  trit_array_p hash1 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  trit_array_p hash2 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_2);
  get_tips_res_t* tips = get_tips_res_new();

  serializer.vtable.get_tips_deserialize_response(&serializer, json_text,
                                                  &tips);
  tmp_tip = get_tips_res_hash_at(tips, 0);
  TEST_ASSERT_EQUAL_MEMORY(tmp_tip->trits, hash1->trits, hash1->num_bytes);
  tmp_tip = get_tips_res_hash_at(tips, 1);
  TEST_ASSERT_EQUAL_MEMORY(tmp_tip->trits, hash2->trits, hash2->num_bytes);
  tmp_tip = get_tips_res_hash_at(tips, 3);
  TEST_ASSERT_NULL(tmp_tip);
  get_tips_res_free(tips);
  trit_array_free(hash1);
  trit_array_free(hash2);
}

void test_serialize_get_trytes(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"getTrytes\",\"hashes\":["
      "\"" TEST_81_TRYRES_1 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  get_trytes_req_t* req = get_trytes_req_new();
  get_trytes_req_add_hash(req, TEST_81_TRYRES_1);

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
      "\"" TEST_2673_TRYRES_1 "\"]}";

  trit_array_p tmp_trytes = NULL;
  trit_array_p hash1 = trit_array_new_from_trytes((tryte_t*)TEST_2673_TRYRES_1);
  get_trytes_res_t* res = get_trytes_res_new();

  serializer.vtable.get_trytes_deserialize_response(&serializer, json_text,
                                                    res);
  tmp_trytes = get_trytes_res_trytes_at(res, 0);
  TEST_ASSERT_EQUAL_MEMORY(tmp_trytes->trits, hash1->trits, hash1->num_bytes);
  tmp_trytes = get_trytes_res_trytes_at(res, 1);
  TEST_ASSERT_NULL(tmp_trytes);

  get_trytes_res_free(&res);
  trit_array_free(hash1);
}

void test_serialize_get_inclusion_states(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getInclusionStates\",\"transactions\":["
      "\"" TEST_81_TRYRES_1 "\"],\"tips\":[\"" TEST_81_TRYRES_2 "\"]}";

  get_inclusion_state_req_t* get_is = get_inclusion_state_req_new();
  char_buffer_t* serializer_out = char_buffer_new();

  get_is = get_inclusion_state_req_add_hash(get_is, TEST_81_TRYRES_1);
  get_is = get_inclusion_state_req_add_tip(get_is, TEST_81_TRYRES_2);

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
  retcode_t ret = RC_OK;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getBalances\",\"addresses\":["
      "\"" TEST_81_TRYRES_1
      "\"]"
      ",\"threshold\":" STR(TEST_BALANCES_SERIALIZE_THRESHOLD) "}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_balances_req_t* get_bal = get_balances_req_new();

  TEST_ASSERT_NOT_NULL(serializer_out);
  TEST_ASSERT_NOT_NULL(get_bal);
  // trytes to flex_trits
  trit_array_p hash1 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  ret = hash243_queue_push(&get_bal->addresses, hash1->trits);
  trit_array_free(hash1);
  TEST_ASSERT(ret == RC_OK);

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
      "[\"" TEST_BALANCES_BALANCE
      "\"], "
      "\"references\": "
      "[\"" TEST_81_TRYRES_1
      "\"], "
      "\"milestoneIndex\":" STR(TEST_BALANCES_MILESTONEINDEX) "}";

  trit_array_p ref = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);

  get_balances_res_t* deserialize_get_bal = get_balances_res_new();
  serializer.vtable.get_balances_deserialize_response(&serializer, json_text,
                                                      deserialize_get_bal);

  TEST_ASSERT_EQUAL_STRING(TEST_BALANCES_BALANCE, get_balances_res_balances_at(
                                                      deserialize_get_bal, 0));

  TEST_ASSERT_EQUAL_MEMORY(ref->trits,
                           hash243_queue_at(&deserialize_get_bal->milestone, 0),
                           ref->num_bytes);

  TEST_ASSERT_EQUAL_INT(TEST_BALANCES_MILESTONEINDEX,
                        deserialize_get_bal->milestoneIndex);

  get_balances_res_free(&deserialize_get_bal);
  trit_array_free(ref);
}

void test_serialize_get_transactions_to_approve(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"getTransactionsToApprove\",\"depth\":" STR(
          TEST_TRANSACTION_TO_APPROVE_DEPTH) ",\"reference\":"
                                             "\"" TEST_81_TRYRES_1 "\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  get_transactions_to_approve_req_t* get_txn_approve =
      get_transactions_to_approve_req_new();
  get_transactions_to_approve_req_set_reference(get_txn_approve,
                                                TEST_81_TRYRES_1);
  get_txn_approve->depth = TEST_TRANSACTION_TO_APPROVE_DEPTH;
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
      "\"" TEST_81_TRYRES_1
      "\",\"branchTransaction\":"
      "\"" TEST_81_TRYRES_2 "\"}";

  trit_array_p trunk = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  trit_array_p branch = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_2);
  get_transactions_to_approve_res_t* deserialize_get_txn_approve =
      get_transactions_to_approve_res_new();
  serializer.vtable.get_transactions_to_approve_deserialize_response(
      &serializer, json_text, &deserialize_get_txn_approve);

  TEST_ASSERT_EQUAL_MEMORY(trunk->trits,
                           deserialize_get_txn_approve->trunk->trits,
                           trunk->num_bytes);
  TEST_ASSERT_EQUAL_MEMORY(branch->trits,
                           deserialize_get_txn_approve->branch->trits,
                           branch->num_bytes);

  get_transactions_to_approve_res_free(&deserialize_get_txn_approve);
  trit_array_free(trunk);
  trit_array_free(branch);
}

void test_serialize_attach_to_tangle(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"attachToTangle\",\"trunkTransaction\":\"" TEST_81_TRYRES_1
      "\",\"branchTransaction\":\"" TEST_81_TRYRES_2
      "\",\"minWeightMagnitude\":" STR(
          TEST_MWM) ",\"trytes\":[\"" TEST_2673_TRYRES_1
                    "\",\"" TEST_2673_TRYRES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  attach_to_tangle_req_t* attach_req = attach_to_tangle_req_new();

  attach_to_tangle_req_set_trunk(attach_req, TEST_81_TRYRES_1);
  attach_to_tangle_req_set_branch(attach_req, TEST_81_TRYRES_2);
  attach_req->trytes =
      attach_to_tangle_req_add_trytes(attach_req->trytes, TEST_2673_TRYRES_1);
  attach_req->trytes =
      attach_to_tangle_req_add_trytes(attach_req->trytes, TEST_2673_TRYRES_2);
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
  const char* json_text = "{\"trytes\":[\"" TEST_81_TRYRES_1
                          "\",\"" TEST_81_TRYRES_2 "\"],\"duration\":4}";
  trit_array_p tmp_hash = NULL;
  trit_array_p hash1 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_1);
  trit_array_p hash2 = trit_array_new_from_trytes((tryte_t*)TEST_81_TRYRES_2);

  attach_to_tangle_res_t* trytes = attach_to_tangle_res_new();

  serializer.vtable.attach_to_tangle_deserialize_response(&serializer,
                                                          json_text, &trytes);

  tmp_hash = attach_to_tangle_res_trytes_at(trytes, 0);
  TEST_ASSERT_EQUAL_MEMORY(hash1->trits, tmp_hash->trits, hash1->num_bytes);

  tmp_hash = attach_to_tangle_res_trytes_at(trytes, 1);
  TEST_ASSERT_EQUAL_MEMORY(hash2->trits, tmp_hash->trits, hash2->num_bytes);

  tmp_hash = attach_to_tangle_res_trytes_at(trytes, 3);
  TEST_ASSERT_NULL(tmp_hash);

  attach_to_tangle_res_free(trytes);
  trit_array_free(hash1);
  trit_array_free(hash2);
}

void test_serialize_broadcast_transactions(void) {
  serializer_t serializer;
  const char* json_text =
      "{\"command\":\"broadcastTransactions\",\"trytes\":["
      "\"" TEST_2673_TRYRES_3 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  broadcast_transactions_req_t* req = broadcast_transactions_req_new();
  broadcast_transactions_req_add_trytes(req, TEST_2673_TRYRES_3);

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
      "\"" TEST_2673_TRYRES_1 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  store_transactions_req_t* req = store_transactions_req_new();
  store_transactions_req_add_trytes(req, TEST_2673_TRYRES_1);

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
      "[\"" TEST_81_TRYRES_1
      "\","
      "\"" TEST_81_TRYRES_2 "\"]}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);
  check_consistency_req_t* req = check_consistency_req_new();
  req = check_consistency_req_add(req, TEST_81_TRYRES_1);
  req = check_consistency_req_add(req, TEST_81_TRYRES_2);

  serializer.vtable.check_consistency_serialize_request(&serializer, req,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  check_consistency_req_free(req);
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
