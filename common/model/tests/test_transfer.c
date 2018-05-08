/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://gitlab.com/iota-foundation/software/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <string.h>
#include <stdlib.h>
#include <unity/unity.h>

#include "common/model/transfer.h"

void test_bundle_hash(coid) {
  transfer_t transfer = transfer_new(VALUE_IN);
  transfer_value_in_t value_in = transfer_value_in(transfer);
  transfer_value_in_set_address(value_in, (const tryte_t *)"9XV9RJGFJJZWITDPKSQXRTHCKJAIZZY9BYLBEQUXUNCLITRQDR9CCD99AANMXYEKD9GLJGVB9HIAGRIBQ");
  transfer_set_tag(transfer, (tryte_t *)"ZTDIDNQDJZGUQKOWJ9JZRCKOVGP");
  transfer_set_timestamp(transfer, 1509136296);

  transfer_t transfers[1] = {transfer};
  Kerl kerl = {};
  init_kerl(&kerl);
  transfer_ctx_t transfer_ctx = transfer_ctx_new();
  transfer_ctx_init(transfer_ctx, transfers, 1);
  transfer_ctx_hash(transfer_ctx, &kerl, transfers, 1);
  tryte_t *bundle_hash = transfer_ctx_finalize(transfer_ctx);
  TEST_ASSERT_EQUAL_MEMORY("NYSJSEGCWESDAFLIFCNJFWGZ9PCYDOT9VCSALKBD9UUNKBJAJCB9KVMTHZDPRDDXC9UFJQBJBQFUPJKFC", bundle_hash, 81);
  transfer_ctx_free(transfer_ctx);
  transfer_free(transfer);
}

int main(void) {
  UNITY_BEGIN();

  RUN_TEST(test_bundle_hash);

  return UNITY_END();
}
