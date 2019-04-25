/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/tests/shared.h"

static void test_response(void) {
  serializer_t serializer;
  char const* json = "{\"error\":\"" TEST_ERROR "\"}";
  error_res_t* res = error_res_new(TEST_ERROR);
  char_buffer_t* serializer_out = char_buffer_new();

  init_json_serializer(&serializer);

  serializer.vtable.error_serialize_response(res, serializer_out);
  TEST_ASSERT_EQUAL_STRING(json, serializer_out->data);

  char_buffer_free(serializer_out);
  error_res_free(&res);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_response);

  return UNITY_END();
}
