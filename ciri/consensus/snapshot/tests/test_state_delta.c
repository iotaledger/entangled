/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <unity/unity.h>

#include "ciri/consensus/snapshot/snapshot.h"
#include "common/model/transaction.h"

static char *snapshot_conf_path = "ciri/consensus/snapshot/tests/snapshot_conf.json";

snapshot_t snapshot;
iota_consensus_conf_t conf;

void test_delta_serialization() {
  state_delta_t delta_deserialized = NULL;
  size_t serialized_size;
  char *buffer;

  strcpy(conf.snapshot_file, "ciri/consensus/snapshot/tests/snapshot.txt");
  TEST_ASSERT(iota_snapshot_init(&snapshot, &conf) == RC_OK);

  serialized_size = state_delta_serialized_str_size(snapshot.state);
  buffer = calloc(serialized_size, sizeof(char));
  state_delta_serialize_str(snapshot.state, buffer);
  state_delta_deserialize_str(buffer, &delta_deserialized);
  TEST_ASSERT(state_delta_equal(snapshot.state, delta_deserialized));
  state_delta_destroy(&snapshot.state);
  state_delta_destroy(&delta_deserialized);
  free(buffer);
}

int main() {
  UNITY_BEGIN();

  iota_consensus_conf_init(&conf);
  strcpy(conf.snapshot_conf_file, snapshot_conf_path);
  conf.snapshot_signature_skip_validation = true;

  RUN_TEST(test_delta_serialization);

  return UNITY_END();
}
