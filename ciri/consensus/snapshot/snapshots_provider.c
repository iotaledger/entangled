/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/consensus/snapshot/snapshots_provider.h"
#include <stdlib.h>

retcode_t iota_snapshots_provider_init(snapshots_provider_t *snapshots_provider, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  ERR_BIND_GOTO(iota_snapshot_reset(&snapshots_provider->inital_snapshot, conf), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshot_init(&snapshots_provider->inital_snapshot, conf), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshot_reset(&snapshots_provider->latest_snapshot, conf), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshot_copy(&snapshots_provider->inital_snapshot, &snapshots_provider->latest_snapshot), ret,
                cleanup);

cleanup:
  if (ret) {
    iota_snapshot_destroy(&snapshots_provider->inital_snapshot);
    iota_snapshot_destroy(&snapshots_provider->latest_snapshot);
  }

  return ret;
}

retcode_t iota_snapshots_provider_destroy(snapshots_provider_t *const snapshots_provider) {
  retcode_t ret = RC_OK;
  ERR_BIND_RETURN(iota_snapshot_destroy(&snapshots_provider->inital_snapshot), ret);
  ERR_BIND_RETURN(iota_snapshot_destroy(&snapshots_provider->latest_snapshot), ret);

  return RC_OK;
}

retcode_t iota_snapshots_provider_write_snapshot_to_file(snapshot_t *const snapshot, char const *const file_path_base) {
  retcode_t ret = RC_OK;
  iota_snapshot_read_lock(snapshot);

  ERR_BIND_GOTO(iota_snapshot_write_to_file(snapshot, file_path_base), ret, cleanup);

cleanup:
  iota_snapshot_unlock(snapshot);

  return ret;
}
