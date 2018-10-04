/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "common/helpers/digest.h"
#include "consensus/snapshot/snapshot.h"
#include "consensus/snapshot/state_diff.h"

// TODO Remove after "Common definitions #329" is merged
#define HASH_LENGTH 243

void test_serialize_deserialize_range() {
  state_map_t state_diff1 = NULL, state_diff2 = NULL;
  state_entry_t *new = NULL, *iter = NULL, *tmp = NULL;
  trit_t trits[HASH_LENGTH] = {1};
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  flex_trit_t *hashed_hash;

  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  for (int i = -1000; i <= 1000; i++) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    new = malloc(sizeof(state_entry_t));
    memcpy(new->hash, hash, FLEX_TRIT_SIZE_243);
    new->value = i;
    HASH_ADD(hh, state_diff1, hash, FLEX_TRIT_SIZE_243, new);
  }

  size_t size = iota_state_diff_serialized_size(&state_diff1);
  byte_t *bytes = calloc(size, sizeof(byte_t));
  TEST_ASSERT(iota_state_diff_serialize(&state_diff1, bytes) == RC_OK);
  TEST_ASSERT(iota_state_diff_deserialize(bytes, size, &state_diff2) == RC_OK);

  int i = -1000;
  flex_trits_from_trits(hash, HASH_LENGTH, trits, HASH_LENGTH, HASH_LENGTH);
  HASH_ITER(hh, state_diff2, iter, tmp) {
    hashed_hash = iota_flex_digest(hash, HASH_LENGTH);
    memcpy(hash, hashed_hash, FLEX_TRIT_SIZE_243);
    free(hashed_hash);
    TEST_ASSERT_EQUAL_MEMORY(iter->hash, hash, FLEX_TRIT_SIZE_243);
    TEST_ASSERT_EQUAL_INT(iter->value, i);
    i++;
  }

  TEST_ASSERT(iota_snapshot_state_destroy(&state_diff1) == RC_OK);
  TEST_ASSERT(iota_snapshot_state_destroy(&state_diff2) == RC_OK);
  free(bytes);
}

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_serialize_deserialize_range);

  return UNITY_END();
}
