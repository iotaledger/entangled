/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <unity/unity.h>

#include "common/model/transaction.h"
#include "consensus/snapshot/snapshot_metadata.h"
#include "utils/time.h"

void test_snapshot_metadata_serialization() {
  snapshot_metadata_t metadata_orig, metadata_deserialized;
  size_t serialized_size;
  char *buffer;
  uint64_t index = 0;
  tryte_t current_address_trytes[NUM_TRYTES_ADDRESS];
  flex_trit_t current_address[FLEX_TRIT_SIZE_243];

  iota_snapshot_metadata_init(&metadata_orig);
  iota_snapshot_metadata_init(&metadata_deserialized);

  metadata_orig.timestamp = current_timestamp_ms();
  metadata_orig.index = rand();
  flex_trits_from_trytes(metadata_orig.hash, NUM_TRITS_HASH,
                           (tryte_t*)"ZZ999999999999999999999999999999999999999999999999999"
                                     "9999999999999999999999999999",
                           NUM_TRYTES_HASH, NUM_TRYTES_HASH);

  strcpy(current_address_trytes,
         "A9999999999999999999999999999999999999999999999999999"
         "9999999999999999999999999999");
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

int main(int argc, char *argv[]) {
  UNITY_BEGIN();

  RUN_TEST(test_snapshot_metadata_serialization);

  return UNITY_END();
}
