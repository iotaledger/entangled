/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_serialize_get_node_api_conf_request(void) {
  serializer_t serializer;
  char const* json_text = "{\"command\":\"getNodeAPIConfiguration\"}";

  char_buffer_t* serializer_out = char_buffer_new();
  init_json_serializer(&serializer);

  serializer.vtable.get_node_api_conf_serialize_request(serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

static void test_deserialize_get_node_api_conf_response(void) {
  serializer_t serializer;
  char_buffer_t* serializer_out = char_buffer_new();

  init_json_serializer(&serializer);
  char const* json_text =
      "{"
        "\"maxFindTransactions\":" STR(TEST_NODE_CONF_MAX_TX) ","
        "\"maxRequestsList\":" STR(TEST_NODE_CONF_MAX_REQ_LIST) ","
        "\"maxGetTrytes\":" STR(TEST_NODE_CONF_MAX_TRYTES) ","
        "\"maxBodyLength\":" STR(TEST_NODE_CONF_MAX_BODY_LEN) ","
        "\"milestoneStartIndex\":" STR(TEST_NODE_CONF_MS_IDX) ","
        "\"testNet\":false"
      "}";

  get_node_api_conf_res_t node_conf = {};

  serializer.vtable.get_node_api_conf_deserialize_response(json_text, &node_conf);

  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MAX_TX, node_conf.max_find_transactions);
  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MAX_REQ_LIST, node_conf.max_requests_list);
  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MAX_TRYTES, node_conf.max_get_trytes);
  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MAX_BODY_LEN, node_conf.max_body_length);
  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MS_IDX, node_conf.milestone_start_index);
  TEST_ASSERT_EQUAL_UINT32(TEST_NODE_CONF_MS_IDX, node_conf.milestone_start_index);
  TEST_ASSERT_EQUAL_INT(0, node_conf.test_net);

  serializer.vtable.get_node_api_conf_serialize_response(&node_conf, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_get_node_api_conf_request);
  RUN_TEST(test_deserialize_get_node_api_conf_response);
  return UNITY_END();
}
