/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <unity/unity.h>

#include "ciri/consensus/snapshot/snapshot_metadata.h"
#include "common/model/transaction.h"
#include "utils/handles/rand.h"
#include "utils/time.h"

static tryte_t *TEST_NULL_HASH_TRYTES =
    (tryte_t *)"999999999999999999999999999999999999999999999999999999999999999999999999999999999";

void test_metadata_serialization() {
  snapshot_metadata_t metadata_orig, metadata_deserialized;
  size_t serialized_size;
  char *buffer;
  uint64_t index = 0;
  tryte_t current_address_trytes[NUM_TRYTES_ADDRESS + 1];
  flex_trit_t current_address[FLEX_TRIT_SIZE_243];
  flex_trit_t null_hash[FLEX_TRIT_SIZE_243];
  flex_trits_from_trytes(null_hash, NUM_TRITS_HASH, TEST_NULL_HASH_TRYTES, NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  iota_snapshot_metadata_init(&metadata_orig, null_hash, rand_handle_rand(), current_timestamp_ms() / 1000, NULL);
  iota_snapshot_metadata_init(&metadata_deserialized, null_hash, 0, 0, NULL);

  flex_trits_from_trytes(metadata_orig.hash, NUM_TRITS_HASH,
                           (tryte_t*)"ZZ999999999999999999999999999999999999999999999999999"
                                     "9999999999999999999999999999",
                           NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  strcpy((char *)current_address_trytes,
         "A99999999999999999999999999999999999999999999999999999999999999999999999999999999");
  for (index = 0; index < 20; ++index) {
    flex_trits_from_trytes(current_address, NUM_TRITS_HASH, current_address_trytes, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
    hash_to_uint64_t_map_add(&metadata_orig.solid_entry_points, current_address, index);
    current_address_trytes[0] += 1;
  }

  serialized_size = iota_snapshot_metadata_serialized_str_size(&metadata_orig);
  buffer = calloc(serialized_size, sizeof(char));

  TEST_ASSERT(iota_snapshot_metadata_serialize_str(&metadata_orig, buffer) == RC_OK);
  TEST_ASSERT(iota_snapshot_metadata_deserialize_str(buffer, &metadata_deserialized) == RC_OK);

  TEST_ASSERT_EQUAL_INT64(metadata_orig.index, metadata_deserialized.index);
  TEST_ASSERT_EQUAL_INT64(metadata_orig.timestamp, metadata_deserialized.timestamp);
  TEST_ASSERT_EQUAL_MEMORY(metadata_orig.hash, metadata_deserialized.hash, FLEX_TRIT_SIZE_243);
  TEST_ASSERT(hash_to_uint64_t_map_equal(metadata_orig.solid_entry_points, metadata_deserialized.solid_entry_points));

  iota_snapshot_metadata_destroy(&metadata_orig);
  iota_snapshot_metadata_destroy(&metadata_deserialized);

  free(buffer);
}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_metadata_serialization);

  return UNITY_END();
}
