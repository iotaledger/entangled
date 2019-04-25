/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_serialize_get_node_info_request(void) {
  serializer_t serializer;
  char const* json_text = "{\"command\":\"getNodeInfo\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_node_info_serialize_request(serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

static void test_deserialize_get_node_info_response(void) {
  serializer_t serializer;
  char_buffer_t* serializer_out = char_buffer_new();

  init_json_serializer(&serializer);
  char const* json_text =
      "{"
        "\"appName\":\"" TEST_INFO_APP_NAME "\","
        "\"appVersion\":\"" TEST_INFO_APP_VERSION "\","
        "\"latestMilestone\":\"" TEST_81_TRYTES_1 "\","
        "\"latestMilestoneIndex\":" STR(TEST_INFO_LATEST_MILESTONE_INDEX) ","
        "\"latestSolidSubtangleMilestone\":\"" TEST_81_TRYTES_2 "\","
        "\"latestSolidSubtangleMilestoneIndex\":" STR(TEST_INFO_LATEST_SS_MILESTONE_INDEX) ","
        "\"milestoneStartIndex\":" STR(TEST_INFO_MILESTONE_START_INDEX) ","
        "\"neighbors\":" STR(TEST_INFO_NEIGHBORS) ","
        "\"packetsQueueSize\":" STR(TEST_INFO_PACKETS_QUEUE_SIZE) ","
        "\"time\":" STR(TEST_INFO_TIME) ","
        "\"tips\":" STR(TEST_INFO_TIPS) ","
        "\"transactionsToRequest\":" STR(TEST_INFO_TRANSACTIONS_TO_REQUEST) ","
        "\"features\":[\"" TEST_INFO_FEATURE_SNAPSHOTPRUNING "\",\"" TEST_INFO_FEATURE_TESTNET "\"],"
        "\"coordinatorAddress\":\"" TEST_81_TRYTES_3 "\""
      "}";
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(json_text, node_info);

  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_NAME, get_node_info_res_app_name(node_info));
  TEST_ASSERT_EQUAL_STRING(TEST_INFO_APP_VERSION, get_node_info_res_app_version(node_info));

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_1, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, get_node_info_res_lm(node_info), FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_UINT32(TEST_INFO_LATEST_MILESTONE_INDEX, node_info->latest_milestone_index);

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_2, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, get_node_info_res_lssm(node_info), FLEX_TRIT_SIZE_243);

  TEST_ASSERT_EQUAL_UINT32(TEST_INFO_LATEST_SS_MILESTONE_INDEX, node_info->latest_solid_subtangle_milestone_index);
  TEST_ASSERT_EQUAL_UINT32(TEST_INFO_MILESTONE_START_INDEX, node_info->milestone_start_index);
  TEST_ASSERT_EQUAL_UINT16(TEST_INFO_NEIGHBORS, node_info->neighbors);
  TEST_ASSERT_EQUAL_UINT16(TEST_INFO_PACKETS_QUEUE_SIZE, node_info->packets_queue_size);
  TEST_ASSERT_EQUAL_UINT64(TEST_INFO_TIME, node_info->time);
  TEST_ASSERT_EQUAL_UINT32(TEST_INFO_TIPS, node_info->tips);
  TEST_ASSERT_EQUAL_UINT32(TEST_INFO_TRANSACTIONS_TO_REQUEST, node_info->transactions_to_request);

  TEST_ASSERT_EQUAL_STRING(TEST_INFO_FEATURE_SNAPSHOTPRUNING, get_node_info_res_features_at(node_info, 0));
  TEST_ASSERT_EQUAL_STRING(TEST_INFO_FEATURE_TESTNET, get_node_info_res_features_at(node_info, 1));

  flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)TEST_81_TRYTES_3, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
  TEST_ASSERT_EQUAL_MEMORY(hash, get_node_info_res_coordinator_address(node_info), FLEX_TRIT_SIZE_243);

  serializer.vtable.get_node_info_serialize_response(node_info, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  get_node_info_res_free(&node_info);
  char_buffer_free(serializer_out);
}
int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_get_node_info_request);
  RUN_TEST(test_deserialize_get_node_info_response);
  return UNITY_END();
}
