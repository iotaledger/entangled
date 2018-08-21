#include <stdlib.h>
#include <string.h>
#include <unity/unity.h>

#include "serializer/json/json_serializer.h"

void test_serialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"command\":\"findTransactions\",\"addresses\":["
      "\"RVORZ9SIIP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT"
      "9XNMVKWYGVAZETAIRPTM\"],\"approvees\":["
      "\"LCIKYSBE9IHXLIKCEJTTIQOTTAWSQCCQQ9A9VOKIWRBWVPXMCGUENWVVMQAMPEIVHEQ9JX"
      "LCNZOORVZTZ\"],\"bundles\":["
      "\"VUDHGOXOYEVRPUHAWOUEPHUXVTTUTDUJFFJNBFGWISXQSNRVBUHKETTMEPBXYXEPJAVVX9"
      "LKUSJNAECEW\"],\"tags\":[\"MINEIOTADOTCOM9999999999999\"]}";

  find_transactions_req_t* find_tran = find_transactions_req_new();
  char_buffer* serializer_out = char_buffer_new();

  find_transactions_req_add_tag(find_tran, "MINEIOTADOTCOM9999999999999");
  find_transactions_req_add_approvee(
      find_tran,
      "LCIKYSBE9IHXLIKCEJTTIQOTTAWSQCCQQ9A9VOKIWRBWVPXMCGUENWVVMQAMPEIVHEQ9JXLC"
      "NZOORVZTZ");
  find_transactions_req_add_address(
      find_tran,
      "RVORZ9SIIP9RCYMREUIXXVPQIPHVCNPQ9HZWYKFWYWZRE9JQKG9REPKIASHUUECPSQO9JT9X"
      "NMVKWYGVAZETAIRPTM");
  find_transactions_req_add_bundle(find_tran,
                                   "VUDHGOXOYEVRPUHAWOUEPHUXVTTUTDUJFFJNBFGWISX"
                                   "QSNRVBUHKETTMEPBXYXEPJAVVX9LKUSJNAECEW");

  serializer.vtable.find_transactions_serialize_request(&serializer, find_tran,
                                                        serializer_out);

  TEST_ASSERT_EQUAL_STRING(json_text, serializer_out->data);

  char_buffer_free(serializer_out);
  find_transactions_req_free(find_tran);
}

void test_deserialize_find_transactions(void) {
  serializer_t serializer;
  init_json_serializer(&serializer);
  const char* json_text =
      "{\"hashes\": "
      "[\"ZJVYUGTDRPDYFGFXMKOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGML"
      "JUFLZWSY9999\", "
      "\"9CZ9KJZCDJIKWARATSNPHNWPUM9WHNTCQLZKYQBZIWMXIQXFFKMMHLXIBYXZBALCUVD9X9"
      "NTNVAGNWGUW\", "
      "\"ZJVYUGTDRPDYF9999KOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGMLJ"
      "UFLZWSY9999\"]}";

  find_transactions_res_t* deserialize_find_tran = find_transactions_res_new();

  serializer.vtable.find_transactions_deserialize_response(
      &serializer, json_text, deserialize_find_tran);
  TEST_ASSERT_EQUAL_STRING(
      "ZJVYUGTDRPDYFGFXMKOTV9ZWSGFK9CFPXTITQLQNLPPG9YNAARMKNKYQO9GSCSBIOTGMLJUF"
      "LZWSY9999",
      find_transactions_res_hash_at(deserialize_find_tran, 0));
  TEST_ASSERT_EQUAL_STRING(
      "9CZ9KJZCDJIKWARATSNPHNWPUM9WHNTCQLZKYQBZIWMXIQXFFKMMHLXIBYXZBALCUVD9X9NT"
      "NVAGNWGUW",
      find_transactions_res_hash_at(deserialize_find_tran, 1));
  TEST_ASSERT_EQUAL_STRING(
      NULL, find_transactions_res_hash_at(deserialize_find_tran, 3));
  find_transactions_res_free(deserialize_find_tran);
}

void test_serialize_get_node_info(void) {
  serializer_t serializer;
  const char* json_text = "{\"command\":\"getNodeInfo\"}";

  char_buffer* serializer_out = char_buffer_new();
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
      "{\"appName\":\"IRI\",\"appVersion\":\"1.0.8.nu\",\"duration\":1,"
      "\"jreAvailableProcessors\":4,\"jreFreeMemory\":91707424,"
      "\"jreMaxMemory\":1908932608,\"jreTotalMemory\":122683392,"
      "\"latestMilestone\":"
      "\"VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOH"
      "KUQIKOY9999\",\"latestMilestoneIndex\":107,"
      "\"latestSolidSubtangleMilestone\":"
      "\"VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOH"
      "KUQIKOY9999\",\"latestSolidSubtangleMilestoneIndex\":107,\"neighbors\":"
      "2,\"packetsQueueSize\":0,\"time\":1477037811737,\"tips\":3,"
      "\"transactionsToRequest\":0}";

  get_node_info_res_t* node_info = get_node_info_res_new();

  serializer.vtable.get_node_info_deserialize_response(&serializer, json_text,
                                                       node_info);

  TEST_ASSERT_EQUAL_STRING("IRI", node_info->appName->data);
  TEST_ASSERT_EQUAL_STRING("1.0.8.nu", node_info->appVersion->data);
  TEST_ASSERT_EQUAL_INT(4, node_info->jreAvailableProcessors);
  TEST_ASSERT_EQUAL_INT8(91707424, node_info->jreFreeMemory);
  TEST_ASSERT_EQUAL_INT8(1908932608, node_info->jreMaxMemory);
  TEST_ASSERT_EQUAL_INT8(122683392, node_info->jreTotalMemory);
  TEST_ASSERT_EQUAL_STRING(
      "VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOHKU"
      "QIKOY9999",
      node_info->latestMilestone->data);
  TEST_ASSERT_EQUAL_INT8(107, node_info->latestMilestoneIndex);
  TEST_ASSERT_EQUAL_STRING(
      "VBVEUQYE99LFWHDZRFKTGFHYGDFEAMAEBGUBTTJRFKHCFBRTXFAJQ9XIUEZQCJOQTZNOOHKU"
      "QIKOY9999",
      node_info->latestSolidSubtangleMilestone->data);
  TEST_ASSERT_EQUAL_INT8(107, node_info->latestSolidSubtangleMilestoneIndex);
  TEST_ASSERT_EQUAL_INT(2, node_info->neighbors);
  TEST_ASSERT_EQUAL_INT(0, node_info->packetsQueueSize);
  TEST_ASSERT_EQUAL_INT8(1477037811737, node_info->time);
  TEST_ASSERT_EQUAL_INT(3, node_info->tips);
  TEST_ASSERT_EQUAL_INT(0, node_info->transactionsToRequest);

  get_node_info_res_free(node_info);
}
int main(void) {
  UNITY_BEGIN();

  // find_transactions
  RUN_TEST(test_serialize_find_transactions);
  RUN_TEST(test_deserialize_find_transactions);

  // get_node_info
  RUN_TEST(test_serialize_get_node_info);
  RUN_TEST(test_deserialize_get_node_info);

  return UNITY_END();
}
