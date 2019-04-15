/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot_metadata.h"

retcode_t iota_snapshot_metadata_init(snapshot_metadata_t* const snapshot_metadata) {
  snapshot_metadata->solid_entry_points = NULL;
  return RC_OK;
}

retcode_t iota_snapshot_metadata_destroy(snapshot_metadata_t* const snapshot_metadata) {
  hash_to_uint64_t_map_free(&snapshot_metadata->solid_entry_points);
  return RC_OK;
}