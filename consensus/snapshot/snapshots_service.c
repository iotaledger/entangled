/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshots_service.h"
#include <inttypes.h>
#include <stdlib.h>
#include "consensus/utils/tangle_traversals.h"
#include "utils/logger_helper.h"
#include "utils/time.h"

#define SNAPSHOTS_SERVICE_LOGGER_ID "snapshots_service"

static logger_id_t logger_id;

static retcode_t check_transaction_is_not_orphan_add_entry_point_do_func(flex_trit_t *hash, iota_stor_pack_t *pack,
                                                                         void *data, bool *should_branch,
                                                                         bool *should_stop);

static retcode_t find_solid_entry_points_and_update_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                            bool *should_branch, bool *should_stop);

static retcode_t iota_snapshots_service_update_new_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                                      snapshot_t *const initial_snapshot,
                                                                      iota_milestone_t const *const target_milestone,
                                                                      tangle_t const *const tangle,
                                                                      hash_to_uint64_t_map_t *const solid_entry_points);

static retcode_t iota_snapshots_service_update_old_solid_entry_points(snapshots_service_t *const snapshots_service,
                                                                      snapshot_t *const snapshot,
                                                                      iota_milestone_t const *const target_milestone,
                                                                      tangle_t const *const tangle,
                                                                      hash_to_uint64_t_map_t *const solid_entry_points);

static retcode_t iota_snapshots_service_find_solid_entry_points_and_update(
    snapshots_service_t *const snapshots_service, uint64_t target_milestone_index, uint64_t target_milestone_timestamp,
    uint64_t min_snapshot_index, flex_trit_t const *const hash, tangle_t const *const tangle,
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
  bool is_orphan;
} check_not_orphan_do_func_params_t;

retcode_t iota_snapshots_service_init(snapshots_service_t *snapshots_service,
                                      snapshots_provider_t *const snapshots_provider,
                                      milestone_service_t *const milestone_service, iota_consensus_conf_t *conf) {
  retcode_t ret;
  snapshots_service->snapshots_provider = snapshots_provider;
  snapshots_service->milestone_service = milestone_service;
  snapshots_service->conf = conf;
  logger_id = logger_helper_enable(SNAPSHOTS_SERVICE_LOGGER_ID, LOGGER_DEBUG, true);

  connection_config_t db_conf = {.db_path = snapshots_service->conf->db_path};

  return RC_OK;
}

retcode_t iota_snapshots_service_destroy(snapshots_service_t *const snapshots_service) {
  logger_helper_release(logger_id);
  return RC_OK;
}

retcode_t iota_snapshots_service_take_snapshot(snapshots_service_t *const snapshots_service,
                                               milestone_tracker_t const *const milestone_tracker,
                                               tangle_t const *const tangle) {
  retcode_t ret;
  DECLARE_PACK_SINGLE_MILESTONE(milestone, milestone_ptr, pack);

  ERR_BIND_RETURN(iota_snapshots_service_determine_new_entry_point(snapshots_service, &pack, tangle), ret);

  snapshot_t next_snapshot;
  ERR_BIND_GOTO(iota_snapshots_service_generate_snapshot(snapshots_service, &milestone, tangle, &next_snapshot), ret,
                cleanup);
  if (next_snapshot.metadata.index > snapshots_service->snapshots_provider->inital_snapshot.metadata.index) {
    ERR_BIND_GOTO(
        iota_snapshots_service_generate_snapshot_metadata(snapshots_service, &milestone, tangle, &next_snapshot), ret,
        cleanup);
    // TODO - Implement prunning
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
    return RC_SNAPSHOT_SERVICE_NOT_ENOUGH_DEPTH;
  }

  index = snapshots_service->snapshots_provider->latest_snapshot.metadata.index -
          snapshots_service->conf->local_snapshots.min_depth;

  // Make sure snapshots aren't too big
  if ((index - snapshots_service->snapshots_provider->inital_snapshot.metadata.index) >
      SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC) {
    index = snapshots_service->snapshots_provider->inital_snapshot.metadata.index +
            SNAPSHOT_SERVICE_MAX_NUM_MILESTONES_TO_CALC;
  }

  ERR_BIND_RETURN(iota_tangle_milestone_load_previous(tangle, index, entry_point), err);

  if (entry_point->num_loaded == 0) {
    log_error(logger_id, "Target milestone was not loaded\n");
    return RC_SNAPSHOT_SERVICE_MILESTONE_NOT_LOADED;
  }

  return RC_OK;
}

retcode_t iota_snapshots_service_generate_snapshot(snapshots_service_t *const snapshots_service,
                                                   iota_milestone_t const *const target_milestone,
                                                   tangle_t const *const tangle, snapshot_t *const snapshot) {
  retcode_t ret;

  iota_snapshot_read_lock(&snapshots_service->snapshots_provider->inital_snapshot);

  if (target_milestone->index <= snapshots_service->snapshots_provider->inital_snapshot.metadata.index) {
    log_error(logger_id, "Target milestone is too old\n");
    ret = RC_SNAPSHOT_SERVICE_MILESTONE_TOO_OLD;
    goto cleanup;
  }

  ERR_BIND_GOTO(iota_snapshot_reset(snapshot, snapshots_service->conf), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshot_copy(&snapshots_service->snapshots_provider->inital_snapshot, snapshot), ret, cleanup);
  ERR_BIND_GOTO(iota_milestone_service_replay_milestones(tangle, snapshots_service->milestone_service, snapshot,
                                                         target_milestone->index),
                ret, cleanup);

cleanup:
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->inital_snapshot);

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

  log_info(logger_id, "Persisting snapshot with index: %" PRIu64 "\n", snapshot->metadata.index);
  ERR_BIND_GOTO(iota_snapshots_provider_write_snapshot_to_file(
                    snapshot, snapshots_service->conf->local_snapshots.local_snapshots_path_base),
                ret, cleanup);

  iota_snapshot_write_lock(&snapshots_service->snapshots_provider->inital_snapshot);

  ERR_BIND_GOTO(iota_snapshot_copy(snapshot, &snapshots_service->snapshots_provider->inital_snapshot), ret, cleanup);

cleanup:
  iota_snapshot_unlock(&snapshots_service->snapshots_provider->inital_snapshot);
  return ret;
}

static retcode_t check_transaction_is_not_orphan_add_entry_point_do_func(flex_trit_t *hash, iota_stor_pack_t *pack,
                                                                         void *data, bool *should_branch,
                                                                         bool *should_stop) {
  retcode_t ret = RC_OK;
  *should_stop = false;
  *should_branch = false;
  find_solid_entry_points_and_update_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_arrival_timestamp = transaction_arrival_timestamp((iota_transaction_t *)pack->models[0]);

  if (current_arrival_timestamp > params->target_milestone_timestamp * 1000UL) {
    ERR_BIND_GOTO(hash_to_uint64_t_map_add(params->solid_entry_points, hash,
                                           transaction_snapshot_index((iota_transaction_t *)pack->models[0])),
                  ret, cleanup);
    *should_stop = true;
  }

cleanup:
  if (ret) {
    *should_stop = true;
    return ret;
  }

  return RC_OK;
}

static retcode_t check_transaction_is_not_orphan_do_func(flex_trit_t *hash, iota_stor_pack_t *pack, void *data,
                                                         bool *should_branch, bool *should_stop) {
  *should_stop = false;
  *should_branch = false;
  check_not_orphan_do_func_params_t *params = data;

  if (pack->num_loaded == 0) {
    return RC_OK;
  }

  uint64_t current_arrival_timestamp = transaction_arrival_timestamp((iota_transaction_t *)pack->models[0]);

  if (current_arrival_timestamp > params->target_milestone_timestamp * 1000UL) {
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

  // Transaction is was referenced by a newer milestone than "target_milestone", therefor, this is a relevant
  // entry point!
  if (current_snapshot_index > params->target_milestone_index) {
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(params->solid_entry_points, hash, current_snapshot_index), ret);
  } else if (current_snapshot_index >= params->min_snapshot_index) {
    *should_branch = true;
    ERR_BIND_GOTO(tangle_traversal_dfs_to_future(
                      params->tangle, check_transaction_is_not_orphan_add_entry_point_do_func, hash, NULL, params),
                  ret, cleanup);
  }

cleanup:
  if (ret) {
    *should_stop = false;
    return ret;
  }

  return RC_OK;
}

static retcode_t iota_snapshots_service_find_solid_entry_points_and_update(
    snapshots_service_t *const snapshots_service, uint64_t target_milestone_index, uint64_t target_milestone_timestamp,
    uint64_t min_snapshot_index, flex_trit_t const *const hash, tangle_t const *const tangle,
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

  params.target_milestone_timestamp = target_milestone_timestamp;
  params.is_orphan = true;

  ERR_BIND_RETURN(tangle_traversal_dfs_to_future(tangle, check_transaction_is_not_orphan_do_func, hash, NULL, &params),
                  ret);

  if (!params.is_orphan) {
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(solid_entry_points, hash, min_snapshot_index), ret);
  }
  return RC_OK;
}

static retcode_t iota_snapshots_service_update_old_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const snapshot,
    iota_milestone_t const *const target_milestone, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret = RC_OK;
  hash_to_uint64_t_map_entry_t *curr_ep_entry = NULL;
  hash_to_uint64_t_map_entry_t *tmp_ep_entry = NULL;
  DECLARE_PACK_SINGLE_TX(milestone_tx, milestone_tx_p, milestone_tx_pack);
  ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, target_milestone->hash, &milestone_tx_pack);
  if (milestone_tx_pack.num_loaded == 0) {
    return RC_SNAPSHOT_MISSING_MILESTONE_TRANSACTION;
  }

  HASH_ITER(hh, snapshot->metadata.solid_entry_points, curr_ep_entry, tmp_ep_entry) {
    if ((memcmp(curr_ep_entry->hash, snapshots_service->conf->genesis_hash, FLEX_TRIT_SIZE_243) != 0) &&
        (target_milestone->index - curr_ep_entry->value) < SNAPSHOT_SERVICE_SOLID_ENTRY_POINT_MAX_DEPTH) {
      ERR_BIND_RETURN(iota_snapshots_service_add_entry_point_if_not_orphan(
                          target_milestone->index, transaction_timestamp(milestone_tx_p), curr_ep_entry->hash,
                          curr_ep_entry->value, tangle, solid_entry_points),
                      ret);
    }
  }

  return RC_OK;
}

static retcode_t iota_snapshots_service_update_new_solid_entry_points(
    snapshots_service_t *const snapshots_service, snapshot_t *const initial_snapshot,
    iota_milestone_t const *const target_milestone, tangle_t const *const tangle,
    hash_to_uint64_t_map_t *const solid_entry_points) {
  retcode_t ret;
  DECLARE_PACK_SINGLE_MILESTONE(prev_milestone, prev_milestone_ptr, prev_milestone_pack);
  DECLARE_PACK_SINGLE_TX(tx, txp, tx_pack);
  prev_milestone = *target_milestone;
  uint64_t index = prev_milestone.index;
  uint64_t depth = SNAPSHOT_SERVICE_SOLID_ENTRY_POINT_MAX_DEPTH;

  ret = iota_tangle_transaction_load(tangle, TRANSACTION_FIELD_HASH, target_milestone->hash, &tx_pack);
  if (tx_pack.num_loaded == 0) {
    return RC_SNAPSHOT_MISSING_MILESTONE_TRANSACTION;
  }

  while (index > initial_snapshot->metadata.index && depth--) {
    ERR_BIND_RETURN(iota_snapshots_service_find_solid_entry_points_and_update(
                        snapshots_service, target_milestone->index, transaction_timestamp(txp), prev_milestone.index,
                        prev_milestone.hash, tangle, solid_entry_points),
                    ret);
    ERR_BIND_RETURN(hash_to_uint64_t_map_add(solid_entry_points, prev_milestone.hash, prev_milestone.index), ret);
    hash_pack_reset(&prev_milestone_pack);
    ERR_BIND_RETURN(iota_tangle_milestone_load_previous(tangle, index, &prev_milestone_pack), ret);
    if (prev_milestone_pack.num_loaded == 0 && index > 1) {
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
  hash_to_uint64_t_map_t *solid_entry_points = &snapshot->metadata.solid_entry_points;

  hash_to_uint64_t_map_remove(solid_entry_points, snapshots_service->conf->genesis_hash);

  ERR_BIND_GOTO(
      hash_to_uint64_t_map_add(solid_entry_points, snapshots_service->conf->genesis_hash, target_milestone->index), ret,
      cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_update_old_solid_entry_points(
                    snapshots_service, &snapshots_service->snapshots_provider->inital_snapshot, target_milestone,
                    tangle, solid_entry_points),
                ret, cleanup);
  ERR_BIND_GOTO(iota_snapshots_service_update_new_solid_entry_points(
                    snapshots_service, &snapshots_service->snapshots_provider->inital_snapshot, target_milestone,
                    tangle, solid_entry_points),
                ret, cleanup);

cleanup:

  return ret;
}
