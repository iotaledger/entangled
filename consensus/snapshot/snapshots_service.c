/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshots_service.h"
#include "utils/logger_helper.h"

#define SNAPSHOTS_SERVICE_LOGGER_ID "snapshots_service"

static logger_id_t logger_id;

retcode_t iota_snapshots_service_init(snapshots_service_t *snapshots_service,
                                      snapshots_provider_t *const snapshots_provider, iota_consensus_conf_t *conf) {
  retcode_t ret;
  snapshots_service->snapshots_provider = snapshots_provider;
  snapshots_service->conf = conf;
  logger_id = logger_helper_enable(SNAPSHOTS_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  connection_config_t db_conf = {.db_path = snapshots_service->conf->db_path};

  if (strcmp(db_conf.db_path, "") != 0) {
    ERR_BIND_RETURN(iota_tangle_init(&snapshots_service->tangle, &db_conf), ret);
  }

  return RC_OK;
}

retcode_t iota_snapshots_service_destroy(snapshots_service_t *const snapshots_service) {
  if (iota_tangle_destroy(&snapshots_service->tangle) != RC_OK) {
    log_critical(logger_id, "Destroying tangle connection failed\n");
  }

  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_snapshots_service_take_snapshot(snapshots_service_t *const snapshots_service,
                                               milestone_tracker_t const *const milestone_tracker) {
  retcode_t err;

  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  ERR_BIND_RETURN(iota_snapshots_service_determine_new_entry_point(snapshots_service, milestone_tracker, &pack), err);

  if (pack.num_loaded == 0) {
    log_error(logger_id, "Target milestone was not loaded\n");
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MILESTONE_NOT_LOADED;
  }

  snapshot_t next_snapshot;
  ERR_BIND_RETURN(
      iota_snapshots_service_generate_snapshot(snapshots_service, milestone_tracker, &milestone, &next_snapshot), err);
  // TODO - implement + uncomment
  // ERR_BIND_RETURN(iota_tangle_transaction_count(&lsm->tangle, &lsm->last_snapshot_transactions_count),err);
  return RC_CONSENSUS_NOT_IMPLEMENTED;
}

retcode_t iota_snapshots_service_determine_new_entry_point(snapshots_service_t *const snapshots_service,
                                                           milestone_tracker_t const *const milestone_tracker,
                                                           iota_stor_pack_t *const entry_point) {
  retcode_t err;
  uint64_t index =
      milestone_tracker->latest_milestone_index - snapshots_service->conf->local_snapshots.local_snapshot_min_depth - 1;
  if (index == 0) {
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_NOT_ENOUGH_DEPTH;
  }
  ERR_BIND_RETURN(iota_tangle_milestone_load_next(&snapshots_service->tangle, index, entry_point), err);

  return RC_OK;
}

retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   milestone_tracker_t const *const milestone_tracker,
                                                   iota_milestone_t const *const milestone,
                                                   snapshot_t *const snapshot) {
  retcode_t ret;

  if (snapshots_service->snapshots_provider->latest_snapshot.index > milestone->index) {
    log_error(logger_id, "Target milestone is not solid\n");
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MILESTONE_NOT_SOLID;
  } else if (milestone->index < snapshots_service->snapshots_provider->inital_snapshot.index) {
    log_error(logger_id, "Target milestone is too old\n");
    return RC_SNAPSHOT_LOCAL_SNAPSHOTS_MILESTONE_TOO_OLD;
  }

  ERR_BIND_RETURN(iota_snapshot_copy(&snapshots_service->snapshots_provider->inital_snapshot, snapshot), ret);

  return RC_OK;
}
