/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshots_provider.h"
#include <stdlib.h>

retcode_t iota_snapshots_provider_init(snapshots_provider_t *snapshots_provider,
                                       iota_consensus_conf_t const *const conf) {
  retcode_t err;
  ERR_BIND_RETURN(iota_snapshot_init(&snapshots_provider->inital_snapshot, conf), err);
  ERR_BIND_RETURN(iota_snapshot_copy(&snapshots_provider->inital_snapshot, &snapshots_provider->latest_snapshot), err);
  return RC_OK;
}

retcode_t iota_snapshots_provider_destroy(snapshots_provider_t *const snapshots_provider) {
  retcode_t ret;
  ERR_BIND_RETURN(iota_snapshot_destroy(&snapshots_provider->inital_snapshot), ret);
  ERR_BIND_RETURN(iota_snapshot_destroy(&snapshots_provider->latest_snapshot), ret);
  return RC_OK;
}

retcode_t iota_snapshots_provider_write_snapshot_to_file(snapshots_provider_t *const snapshots_provider,
                                                         snapshot_t const *const snapshot,
                                                         char const *const file_path) {
  retcode_t ret;
  iota_snapshot_lock_read(snapshot);

  ERR_BIND_GOTO(iota_snapshot_write_to_file(snapshot, file_path), ret, cleanup);

  //TODO - implement write_metadata

cleanup:
  iota_snapshot_unlock(snapshot);
  return ret;
}