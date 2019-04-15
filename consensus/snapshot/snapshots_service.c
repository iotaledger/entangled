/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshots_service.h"
#include "consensus/utils/tangle_traversals.h"
#include "utils/logger_helper.h"

#define SNAPSHOTS_SERVICE_LOGGER_ID "snapshots_service"

/**
 * Maximum age in milestones since creation of solid entry points.
 *
 * Since it is possible to artificially keep old solid entry points alive by periodically attaching new transactions
 * to them, we limit the life time of solid entry points and ignore them whenever they become too old. This is a
 * measure against a potential attack vector where somebody might try to blow up the meta data of local snapshots.
 */

#define SOLID_ENTRY_POINT_LIFETIME 1000

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
  retcode_t ret;

  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  ERR_BIND_RETURN(iota_snapshots_service_determine_new_entry_point(snapshots_service, milestone_tracker, &pack), ret);

  if (pack.num_loaded == 0) {
    log_error(logger_id, "Target milestone was not loaded\n");
    return RC_SNAPSHOT_SERVICE_MILESTONE_NOT_LOADED;
  }

  snapshot_t next_snapshot;
  ERR_BIND_RETURN(
      iota_snapshots_service_generate_snapshot(snapshots_service, milestone_tracker, &milestone, &next_snapshot), ret);
  // TODO - implement + uncomment
  ERR_BIND_RETURN(iota_snapshots_service_persist_snapshot(&snapshots_service, &next_snapshot), ret);
  return RC_OK;
}

retcode_t iota_snapshots_service_determine_new_entry_point(snapshots_service_t *const snapshots_service,
                                                           milestone_tracker_t const *const milestone_tracker,
                                                           iota_stor_pack_t *const entry_point) {
  retcode_t err;
  uint64_t index = milestone_tracker->latest_milestone_index - snapshots_service->conf->local_snapshots.min_depth - 1;
  if (index == 0) {
    return RC_SNAPSHOT_SERVICE_NOT_ENOUGH_DEPTH;
  }
  ERR_BIND_RETURN(iota_tangle_milestone_load_next(&snapshots_service->tangle, index, entry_point), err);

  return RC_OK;
}

retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   milestone_tracker_t const *const milestone_tracker,
                                                   iota_milestone_t const *const target_milestone,
                                                   snapshot_t *const snapshot) {
  retcode_t ret;

  iota_snapshot_lock_read(&snapshots_service->snapshots_provider->inital_snapshot);
  iota_snapshot_lock_read(&snapshots_service->snapshots_provider->latest_snapshot);

  if (snapshots_service->snapshots_provider->latest_snapshot.index > target_milestone->index) {
    log_error(logger_id, "Target milestone is not solid\n");
    ret = RC_SNAPSHOT_SERVICE_MILESTONE_NOT_SOLID;
    goto cleanup;
  } else if (target_milestone->index < snapshots_service->snapshots_provider->inital_snapshot.index) {
    log_error(logger_id, "Target milestone is too old\n");
    ret = RC_SNAPSHOT_SERVICE_MILESTONE_TOO_OLD;
    goto cleanup;
  }

  // TODO - implement rollback as well

  ERR_BIND_GOTO(iota_snapshot_copy(&snapshots_service->snapshots_provider->inital_snapshot, snapshot), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_replay_milestones(snapshots_service, &snapshot, target_milestone->index), ret,
                cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_update_solid_entry_points(&snapshots_service, snapshot, target_milestone), ret,
                cleanup);

cleanup:
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->inital_snapshot);
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->latest_snapshot);

  return ret;
}

retcode_t iota_snapshots_service_replay_milestones(snapshots_service_t *const snapshots_service,
                                                   snapshot_t *const snapshot, uint64_t index) {
  retcode_t ret;
  state_delta_t merged_balance_changes;
  state_delta_t current_delta;
  iota_milestone_t *last_applied_milestone = NULL;
  DECLARE_PACK_SINGLE_MILESTONE(current_milestone, current_milestone_ptr, pack);

  for (uint64_t current_milestone_index = snapshot->index + 1; current_milestone_index <= index;
       ++current_milestone_index) {
    ERR_BIND_RETURN(
        iota_tangle_milestone_load_next(&snapshots_service->tangle, current_milestone_index, current_milestone_index),
        ret);
    if (pack.num_loaded == 0) {
      log_error(logger_id, "Target milestone was not loaded\n");
      // TODO - add to skipped milestones
    } else {
      ERR_BIND_RETURN(iota_tangle_state_delta_load(&snapshots_service->tangle, &current_milestone, &current_delta),
                      ret);
      ERR_BIND_RETURN(state_delta_apply_patch(merged_balance_changes, current_delta), ret);
      last_applied_milestone = &current_milestone;
    }
  }

  if (last_applied_milestone != NULL) {
    // TODO - mutex is not need to be taken inside "iota_snapshot_apply_patch" this time
    ERR_BIND_RETURN(iota_snapshot_apply_patch(snapshot, merged_balance_changes, last_applied_milestone->index), ret);
  }
  return RC_OK;
}

retcode_t iota_snapshots_service_persist_snapshot(snapshots_service_t *const snapshots_service,
                                                  snapshot_t *const snapshot) {
  retcode_t ret;
  iota_snapshot_lock_write(&snapshots_service->snapshots_provider->inital_snapshot);
  ERR_BIND_GOTO(state_delta_merge_patch(&snapshots_service->snapshots_provider->inital_snapshot, &snapshot->state), ret,
                cleanup);
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->inital_snapshot);
  ERR_BIND_GOTO(iota_snapshots_provider_write_snapshot_to_file(
                    snapshots_service->snapshots_provider, snapshot,
                    snapshots_service->conf->local_snapshots.local_snapshots_path_base),
                ret, cleanup);

cleanup:
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->inital_snapshot);

  return RC_OK;
}

static retcode_t iota_snapshots_service_update_old_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                                      snapshot_t *const snapshot,
                                                                      iota_milestone_t const *const target_milestone,
                                                                      hash_to_uint64_t_map_t *const dst) {
  retcode_t ret;
  hash_to_uint64_t_map_entry_t *curr_entry = NULL;
  hash_to_uint64_t_map_entry_t *tmp_entry = NULL;

  HASH_ITER(hh, snapshot->metadata.solid_entry_points, curr_entry, tmp_entry) {
    if ((memcmp(curr_entry->hash, snapshots_service->conf->genesis_hash, FLEX_TRIT_SIZE_243) != 0) &&
        (target_milestone->index - curr_entry->value) < SOLID_ENTRY_POINT_LIFETIME) {
      // TODO - check that entry is solid too
      ret = hash_to_uint64_t_map_add(dst, curr_entry->hash, curr_entry->value);
      if (ret != RC_OK) {
        return ret;
      }
    }
  }

  return RC_OK;
}

typedef struct check_is_solid_entry_point_do_func_params_s {
  uint64_t min_snapshot_index;
  uint64_t target_milestone_index;
  uint64_t target_milestone_timestamp;
  snapshot_t *snapshot;
  snapshots_service_t *snapshots_service;
  hash_to_uint64_t_map_t *solid_entry_points;
} check_is_solid_entry_point_do_func_params_t;

static retcode_t check_transaction_is_not_orphan_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                         bool *should_branch, bool *should_stop) {
  retcode_t ret;
  *should_stop = false;
  *should_branch = false;
  check_is_solid_entry_point_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_arrival_timestamp = transaction_arrival_timestamp((iota_transaction_t *)pack->models[0]);

  if (current_arrival_timestamp > params->target_milestone_timestamp) {
    ERR_BIND_GOTO(
        hash_to_uint64_t_map_add(params->solid_entry_points, transaction_hash((iota_transaction_t *)pack->models[0]),
                                 transaction_snapshot_index((iota_transaction_t *)pack->models[0])),
        ret, cleanup);
    *should_stop = true;
  }

cleanup:
  if (ret) {
    *should_stop = false;
    return ret;
  }

  return RC_OK;
}

static retcode_t check_is_solid_entry_point_relevant_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                             bool *should_branch, bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = false;
  check_is_solid_entry_point_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_snapshot_index = transaction_snapshot_index((iota_transaction_t *)pack->models[0]);

  // Transaction is was referenced by a newer milestone than "target_milestone", therefor, this is a relevant
  // entry point!
  if (current_snapshot_index > params->target_milestone_index) {
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(params->solid_entry_points, hash, current_snapshot_index), ret);
  } else if (current_snapshot_index >= params->min_snapshot_index) {
    *should_branch = true;
    ERR_BIND_GOTO(tangle_traversal_dfs_to_future(&params->snapshots_service->tangle,
                                                 check_transaction_is_not_orphan_do_func, hash, NULL, &params),
                  ret, cleanup);
  }

cleanup:
  if (ret) {
    *should_stop = false;
    return ret;
  }

  return RC_OK;
}

static retcode_t iota_snapshots_service_update_new_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const initial_snapshot,
    iota_milestone_t const *const target_milestone, hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret;
  DECLARE_PACK_SINGLE_MILESTONE(prev_milestone, prev_milestone_ptr, pack);
  prev_milestone_ptr = target_milestone;
  uint64_t index = prev_milestone.index;
  check_is_solid_entry_point_do_func_params_t params;

  params.snapshot = initial_snapshot;
  params.target_milestone_index = target_milestone->index;
  params.snapshots_service = snapshots_service;
  params.solid_entry_points = solid_entry_points;

  while (index > initial_snapshot->index) {
    params.min_snapshot_index = prev_milestone.index;
    ERR_BIND_RETURN(
        tangle_traversal_dfs_to_past(&snapshots_service->tangle, check_is_solid_entry_point_relevant_do_func,
                                     prev_milestone.hash, snapshots_service->conf->genesis_hash, NULL, &params),
        ret);
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(solid_entry_points, prev_milestone.hash, prev_milestone.index), ret);
    iota_tangle_milestone_load_previous(&snapshots_service->tangle, index, &pack);
    if (pack.num_loaded == 0) {
      --index;
    } else {
      index = prev_milestone.index;
    }
  }
  return RC_OK;
}

retcode_t iota_snapshots_service_update_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                           snapshot_t *const snapshot,
                                                           iota_milestone_t const *const target_milestone) {
  retcode_t ret;
  hash_to_uint64_t_map_t *solid_entry_points = &snapshot->metadata.solid_entry_points;
  ERR_BIND_RETURN(
      hash_to_uint64_t_map_add(solid_entry_points, snapshots_service->conf->genesis_hash, target_milestone->index),
      ret);
  ERR_BIND_RETURN(iota_snapshots_service_update_old_solid_entry_points(
                      snapshots_service, &snapshots_service->snapshots_provider->inital_snapshot, target_milestone,
                      solid_entry_points),
                  ret);
  ERR_BIND_RETURN(iota_snapshots_service_update_new_solid_entry_points(
                      snapshots_service, &snapshots_service->snapshots_provider->inital_snapshot, target_milestone,
                      solid_entry_points),
                  ret);
  return RC_OK;
}
