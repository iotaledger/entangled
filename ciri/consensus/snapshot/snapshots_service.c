/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "ciri/consensus/snapshot/snapshots_service.h"
#include "ciri/consensus/tangle/traversal.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/time.h"

#define SNAPSHOTS_SERVICE_LOGGER_ID "snapshots_service"

static logger_id_t logger_id;

static retcode_t find_solid_entry_points_and_update_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                            bool *should_branch, bool *should_stop);

static retcode_t iota_snapshots_service_collect_new_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const initial_snapshot,
    iota_milestone_t const *const target_milestone, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points);

static retcode_t iota_snapshots_service_update_old_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                                      snapshot_t *const snapshot,
                                                                      iota_milestone_t const *const target_milestone,
                                                                      tangle_t const *const tangle,
                                                                      hash_to_uint64_t_map_t *const solid_entry_points);

static retcode_t iota_snapshots_service_find_solid_entry_points_and_update(
    snapshots_service_t *const snapshots_service, uint64_t target_milestone_index, uint64_t target_milestone_timestamp,
    flex_trit_t const *const hash, uint64_t min_snapshot_index, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points);

static retcode_t iota_snapshots_service_add_entry_point_if_not_orphan(
    uint64_t target_milestone_index, uint64_t target_milestone_timestamp, flex_trit_t const *const hash,
    uint64_t min_snapshot_index, tangle_t const *const tangle, hash_to_uint64_t_map_t *const solid_entry_points);

typedef struct find_solid_entry_points_and_update_do_func_params_s {
  uint64_t min_snapshot_index;
  uint64_t target_milestone_index;
  uint64_t target_milestone_timestamp;
  snapshots_service_t *snapshots_service;
  hash_to_uint64_t_map_t *solid_entry_points;
  tangle_t const *tangle;
} find_solid_entry_points_and_update_do_func_params_t;

typedef struct check_not_orphan_do_func_params_s {
  uint64_t target_milestone_timestamp;
  hash_to_uint64_t_map_t *solid_entry_points;
  bool is_orphan;
} check_not_orphan_do_func_params_t;

retcode_t iota_snapshots_service_init(snapshots_service_t *snapshots_service,
                                      snapshots_provider_t *const snapshots_provider,
                                      milestone_service_t *const milestone_service, iota_consensus_conf_t *conf) {
  snapshots_service->snapshots_provider = snapshots_provider;
  snapshots_service->milestone_service = milestone_service;
  snapshots_service->conf = conf;
  logger_id = logger_helper_enable(SNAPSHOTS_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  return RC_OK;
}

retcode_t iota_snapshots_service_destroy(snapshots_service_t *const snapshots_service) {
  UNUSED(snapshots_service);

  logger_helper_release(logger_id);

  return RC_OK;
}

retcode_t iota_snapshots_service_take_snapshot(snapshots_service_t *const snapshots_service,
                                               pruning_service_t *const ps, tangle_t const *const tangle) {
  retcode_t ret = RC_OK;
  snapshot_t next_snapshot;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  ERR_BIND_RETURN(iota_snapshots_service_determine_new_entry_point(snapshots_service, &pack, tangle), ret);

  ERR_BIND_GOTO(iota_snapshot_reset(&next_snapshot, snapshots_service->conf), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_generate_snapshot(snapshots_service, &milestone, tangle, &next_snapshot), ret,
                cleanup);
  if (next_snapshot.metadata.index > snapshots_service->snapshots_provider->initial_snapshot.metadata.index) {
    ERR_BIND_GOTO(
        iota_snapshots_service_generate_snapshot_metadata(snapshots_service, &milestone, tangle, &next_snapshot), ret,
        cleanup);
    if (snapshots_service->conf->local_snapshots.pruning_is_enabled) {
      iota_local_snapshots_pruning_service_update_current_snapshot(ps, &next_snapshot);
    }
    ERR_BIND_GOTO(iota_snapshots_service_persist_snapshot(snapshots_service, &next_snapshot), ret, cleanup);
  }

cleanup:

  iota_snapshot_destroy(&next_snapshot);

  return ret;
}

retcode_t iota_snapshots_service_determine_new_entry_point(snapshots_service_t *const snapshots_service,
                                                           iota_stor_pack_t *const entry_point,
                                                           tangle_t const *const tangle) {
  retcode_t err;
  uint64_t index;

  if (snapshots_service->snapshots_provider->latest_snapshot.metadata.index <=
      snapshots_service->conf->local_snapshots.min_depth) {
    log_warning(logger_id, "Latest milestone index is %" PRId64 " and is below min depth\n",
                snapshots_service->snapshots_provider->latest_snapshot.metadata.index);
    return RC_SNAPSHOT_SERVICE_NOT_ENOUGH_DEPTH;
  }

  index = snapshots_service->snapshots_provider->latest_snapshot.metadata.index -
          snapshots_service->conf->local_snapshots.min_depth;

  if (index <= snapshots_service->snapshots_provider->initial_snapshot.metadata.index) {
    log_warning(logger_id,
                "New candidate milestone for local snapshot is %" PRId64 " and is before initial snapshot (too old)\n",
                snapshots_service->snapshots_provider->initial_snapshot.metadata.index);
    return RC_SNAPSHOT_SERVICE_MILESTONE_TOO_OLD;
  }

  // Make sure snapshots aren't too big
  if ((index - snapshots_service->snapshots_provider->initial_snapshot.metadata.index) >
      SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC) {
    index = snapshots_service->snapshots_provider->initial_snapshot.metadata.index +
            SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC;
  }

  ERR_BIND_RETURN(iota_tangle_milestone_load_by_index(tangle, index - 1, entry_point), err);

  if (entry_point->num_loaded == 0) {
    log_error(logger_id, "Target milestone was not loaded\n");
    return RC_SNAPSHOT_SERVICE_MILESTONE_NOT_LOADED;
  }

  return RC_OK;
}

retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   iota_milestone_t const *const target_milestone,
                                                   tangle_t const *const tangle, snapshot_t *const snapshot) {
  retcode_t ret = RC_OK;

  iota_snapshot_read_lock(&snapshots_service->snapshots_provider->initial_snapshot);
  if (target_milestone->index <= snapshots_service->snapshots_provider->initial_snapshot.metadata.index) {
    log_error(logger_id, "Target milestone is too old\n");
    ret = RC_SNAPSHOT_SERVICE_MILESTONE_TOO_OLD;
  } else {
    ret = iota_snapshot_copy(&snapshots_service->snapshots_provider->initial_snapshot, snapshot);
  }
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->initial_snapshot);
  if (ret) {
    return ret;
  }
  ERR_BIND_RETURN(iota_milestone_service_replay_milestones(snapshots_service->milestone_service, tangle, snapshot,
                                                           target_milestone->index),
                  ret);

  return ret;
}

retcode_t iota_snapshots_service_generate_snapshot_metadata(snapshots_service_t *const snapshots_service,
                                                            iota_milestone_t const *const target_milestone,
                                                            tangle_t const *const tangle, snapshot_t *const snapshot) {
  retcode_t ret;
  DECLARE_PACK_SINGLE_TX(tx, tx_ptr, tx_pack);

  snapshot->metadata.index = target_milestone->index;
  memcpy(snapshot->metadata.hash, target_milestone->hash, FLEX_TRIT_SIZE_243);
  ERR_BIND_RETURN(iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, target_milestone->hash, &tx_pack), ret);
  snapshot->metadata.timestamp = transaction_timestamp(&tx);
  if ((snapshots_service->snapshots_provider->latest_snapshot.metadata.index - target_milestone->index) <=
      SNAPSHOT_SERVICE_SOLID_ENTRY_POINT_MAX_DEPTH) {
    ERR_BIND_RETURN(
        iota_snapshots_service_update_solid_entry_points(snapshots_service, snapshot, target_milestone, tangle), ret);
  }

  // TODO - seen milestones
  return RC_OK;
}

retcode_t iota_snapshots_service_persist_snapshot(snapshots_service_t *const snapshots_service,
                                                  snapshot_t *const snapshot) {
  retcode_t ret;

  ERR_BIND_RETURN(
      iota_snapshots_provider_write_snapshot_to_file(snapshot, snapshots_service->conf->local_snapshots.base_dir), ret);

  iota_snapshot_write_lock(&snapshots_service->snapshots_provider->initial_snapshot);
  ret = iota_snapshot_copy(snapshot, &snapshots_service->snapshots_provider->initial_snapshot);
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->initial_snapshot);

  return ret;
}

static retcode_t check_transaction_is_not_orphan_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                         bool *should_branch, bool *should_stop) {
  *should_stop = false;
  *should_branch = true;
  check_not_orphan_do_func_params_t *params = data;

  if (pack->num_loaded == 0 || transaction_snapshot_index((iota_transaction_t *)pack->models[0]) != 0) {
    *should_branch = false;
    return RC_OK;
  }

  if (hash_to_uint64_t_map_contains(*params->solid_entry_points, hash)) {
    params->is_orphan = false;
    *should_stop = true;
    return RC_OK;
  }

  uint64_t current_attachment_timestamp = transaction_attachment_timestamp((iota_transaction_t *)pack->models[0]);
  uint64_t current_timestamp = transaction_timestamp((iota_transaction_t *)pack->models[0]);

  if ((current_attachment_timestamp > params->target_milestone_timestamp * 1000UL ||
       current_timestamp > params->target_milestone_timestamp)) {
    params->is_orphan = false;
    *should_stop = true;
  }

  return RC_OK;
}

static retcode_t find_solid_entry_points_and_update_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                            bool *should_branch, bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = false;
  find_solid_entry_points_and_update_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_snapshot_index = transaction_snapshot_index((iota_transaction_t *)pack->models[0]);

  // Transaction was referenced by a newer milestone than "target_milestone", therefor, this is a relevant
  // entry point!
  if (current_snapshot_index > params->target_milestone_index) {
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(params->solid_entry_points, hash, current_snapshot_index), ret);
  }

  if (current_snapshot_index >= params->min_snapshot_index) {
    *should_branch = true;
    ERR_BIND_GOTO(iota_snapshots_service_add_entry_point_if_not_orphan(
                      params->target_milestone_index, params->target_milestone_timestamp, hash,
                      params->min_snapshot_index, params->tangle, params->solid_entry_points),
                  ret, cleanup);
  }

cleanup:
  if (ret) {
    *should_stop = true;
    return ret;
  }

  return RC_OK;
}

static retcode_t iota_snapshots_service_find_solid_entry_points_and_update(
    snapshots_service_t *const snapshots_service, uint64_t target_milestone_index, uint64_t target_milestone_timestamp,
    flex_trit_t const *const hash, uint64_t min_snapshot_index, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret;
  find_solid_entry_points_and_update_do_func_params_t params;

  params.target_milestone_index = target_milestone_index;
  params.snapshots_service = snapshots_service;
  params.solid_entry_points = solid_entry_points;
  params.min_snapshot_index = min_snapshot_index;
  params.target_milestone_timestamp = target_milestone_timestamp;
  params.tangle = tangle;
  ERR_BIND_RETURN(tangle_traversal_dfs_to_past(tangle, find_solid_entry_points_and_update_do_func, hash,
                                               snapshots_service->conf->genesis_hash, NULL, &params),
                  ret);
  return RC_OK;
}

static retcode_t iota_snapshots_service_add_entry_point_if_not_orphan(
    uint64_t target_milestone_index, uint64_t target_milestone_timestamp, flex_trit_t const *const hash,
    uint64_t min_snapshot_index, tangle_t const *const tangle, hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret;
  check_not_orphan_do_func_params_t params;
  iota_stor_pack_t hashes_pack;

  UNUSED(target_milestone_index);
  params.target_milestone_timestamp = target_milestone_timestamp;
  params.solid_entry_points = solid_entry_points;
  params.is_orphan = true;

  ERR_BIND_RETURN(hash_pack_init(&hashes_pack, 8), ret);

  ERR_BIND_GOTO(iota_tangle_transaction_load_hashes_of_approvers(tangle, hash, &hashes_pack, 0), ret, cleanup);

  while (hashes_pack.num_loaded > 0) {
    // Add each found approver to the currently traversed tx
    ERR_BIND_GOTO(
        tangle_traversal_dfs_to_future(tangle, check_transaction_is_not_orphan_do_func,
                                       ((flex_trit_t *)hashes_pack.models[--hashes_pack.num_loaded]), NULL, &params),
        ret, cleanup);

    if (!params.is_orphan) {
      ERR_BIND_GOTO(hash_to_uint64_t_map_add(solid_entry_points, hash, min_snapshot_index), ret, cleanup);
    }
  }

cleanup:
  hash_pack_free(&hashes_pack);

  return RC_OK;
}

static retcode_t iota_snapshots_service_update_old_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const snapshot,
    iota_milestone_t const *const target_milestone, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret = RC_OK;
  hash_to_uint64_t_map_entry_t *curr_ep_entry = NULL;
  hash_to_uint64_t_map_entry_t *tmp_ep_entry = NULL;
  hash_to_uint64_t_map_t snapshot_solid_entry_points = NULL;
  DECLARE_PACK_SINGLE_TX(milestone_tx, milestone_tx_p, milestone_tx_pack);

  iota_snapshot_read_lock(snapshot);
  hash_to_uint64_t_map_copy(&snapshot->metadata.solid_entry_points, &snapshot_solid_entry_points);
  iota_snapshot_unlock(snapshot);

  ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, target_milestone->hash, &milestone_tx_pack);
  if (milestone_tx_pack.num_loaded == 0) {
    ret = RC_SNAPSHOT_MISSING_MILESTONE_TRANSACTION;
    goto cleanup;
  }

  HASH_ITER(hh, snapshot_solid_entry_points, curr_ep_entry, tmp_ep_entry) {
    if ((memcmp(curr_ep_entry->hash, snapshots_service->conf->genesis_hash, FLEX_TRIT_SIZE_243) != 0) &&
        (target_milestone->index - curr_ep_entry->value) < SNAPSHOT_SERVICE_SOLID_ENTRY_POINT_MAX_DEPTH) {
      ERR_BIND_GOTO(iota_snapshots_service_add_entry_point_if_not_orphan(
                        target_milestone->index, transaction_timestamp(milestone_tx_p), curr_ep_entry->hash,
                        curr_ep_entry->value, tangle, solid_entry_points),
                    ret, cleanup);
    }
  }

cleanup:
  hash_to_uint64_t_map_free(&snapshot_solid_entry_points);

  return ret;
}

static retcode_t iota_snapshots_service_collect_new_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const initial_snapshot,
    iota_milestone_t const *const target_milestone, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret;
  DECLARE_PACK_SINGLE_MILESTONE(prev_milestone, prev_milestone_ptr, prev_milestone_pack);
  DECLARE_PACK_SINGLE_TX(target_milestone_tx, target_milestone_tx_p, target_milestone_tx_pack);
  prev_milestone = *target_milestone;
  uint64_t index = prev_milestone.index;

  ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, target_milestone->hash, &target_milestone_tx_pack);
  if (target_milestone_tx_pack.num_loaded == 0) {
    log_warning(logger_id, "Milestone with index (target milestone) %" PRIu64 " could not be loaded\n",
                target_milestone->index);
    return RC_SNAPSHOT_MISSING_MILESTONE_TRANSACTION;
  }

  while (index > initial_snapshot->metadata.index && index > 1) {
    ERR_BIND_RETURN(iota_snapshots_service_find_solid_entry_points_and_update(
                        snapshots_service, target_milestone->index, transaction_timestamp(target_milestone_tx_p),
                        prev_milestone.hash, prev_milestone.index, tangle, solid_entry_points),
                    ret);
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(solid_entry_points, prev_milestone.hash, prev_milestone.index), ret);
    hash_pack_reset(&prev_milestone_pack);
    ERR_BIND_RETURN(iota_tangle_milestone_load_by_index(tangle, index - 1, &prev_milestone_pack), ret);
    if (prev_milestone_pack.num_loaded == 0 && (index - 1) > initial_snapshot->metadata.index) {
      log_warning(logger_id, "Milestone with index %" PRIu64 " could not be loaded\n", index - 1);
      return RC_SNAPSHOT_MISSING_MILESTONE_TRANSACTION;
    } else {
      index = prev_milestone.index;
    }
  }
  return RC_OK;
}

retcode_t iota_snapshots_service_update_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                           snapshot_t *const snapshot,
                                                           iota_milestone_t const *const target_milestone,
                                                           tangle_t const *const tangle) {
  retcode_t ret;
  hash_to_uint64_t_map_t solid_entry_points = NULL;

  ERR_BIND_GOTO(
      hash_to_uint64_t_map_add(&solid_entry_points, snapshots_service->conf->genesis_hash, target_milestone->index),
      ret, cleanup);

  ERR_BIND_GOTO(iota_snapshots_service_update_old_solid_entry_points(
                    snapshots_service, &snapshots_service->snapshots_provider->initial_snapshot, target_milestone,
                    tangle, &solid_entry_points),
                ret, cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_collect_new_solid_entry_points(
                    snapshots_service, &snapshots_service->snapshots_provider->initial_snapshot, target_milestone,
                    tangle, &solid_entry_points),
                ret, cleanup);

cleanup:
  hash_to_uint64_t_map_free(&snapshot->metadata.solid_entry_points);
  hash_to_uint64_t_map_copy(&solid_entry_points, &snapshot->metadata.solid_entry_points);
  hash_to_uint64_t_map_free(&solid_entry_points);

  return ret;
}
