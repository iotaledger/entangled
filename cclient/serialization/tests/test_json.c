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

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_find_transactions);
  RUN_TEST(test_deserialize_find_transactions);

  return UNITY_END();
}
