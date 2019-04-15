/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot_metadata.h"
#include <inttypes.h>
#include "utils/macros.h"

retcode_t iota_snapshot_metadata_init(snapshot_metadata_t *const snapshot_metadata) {
  snapshot_metadata->solid_entry_points = NULL;
  return RC_OK;
}

retcode_t iota_snapshot_metadata_destroy(snapshot_metadata_t *const snapshot_metadata) {
  hash_to_uint64_t_map_free(&snapshot_metadata->solid_entry_points);
  return RC_OK;
}

size_t iota_snapshot_metadata_serialized_str_size(snapshot_metadata_t const *const snapshot_metadata) {
  // hash, index, timestamps each are followed by new line character
  // each solid entry point map entry is two fields delimited by ';' and ended with a new line character
  return FLEX_TRIT_SIZE_243 + 1 + MAX_CHARS_UNIT64 + 1 + MAX_CHARS_UNIT64 + 1 +
         hash_to_uint64_t_map_size(snapshot_metadata->solid_entry_points) *
             (MAX_CHARS_UNIT64 + 1 + FLEX_TRIT_SIZE_243 + 1);
}

retcode_t iota_snapshot_metadata_serialize_str(snapshot_metadata_t const *const snapshot_metadata, char *const str) {
  hash_to_uint64_t_map_entry_t *iter = NULL, *tmp = NULL;
  tryte_t hash_trytes[81];

  flex_trits_to_trytes(&hash_trytes, 81, snapshot_metadata->hash, FLEX_TRIT_SIZE_243, FLEX_TRIT_SIZE_243);
  snprintf(str, 81, "%s\n", hash_trytes);
  sprintf(str, "%" PRIu64 "\n", snapshot_metadata->index);
  sprintf(str, "%" PRIu64 "\n", snapshot_metadata->timestamp);

  HASH_ITER(hh, snapshot_metadata->solid_entry_points, iter, tmp) {
    flex_trits_to_trytes(&hash_trytes, 81, iter->hash, FLEX_TRIT_SIZE_243, FLEX_TRIT_SIZE_243);
    strncpy(str, (char *)hash_trytes, 81);
    strncpy(str, ";", 1);
    sprintf(str, "%" PRIu64 "\n", iter->value);
  }
}