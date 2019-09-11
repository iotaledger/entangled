/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>
#include <stdlib.h>

#include "ciri/consensus/conf.h"
#include "ciri/consensus/snapshot/snapshot.h"
#include "common/model/transaction.h"
#include "utils/files.h"
#include "utils/logger_helper.h"
#include "utils/macros.h"
#include "utils/signed_files.h"

#if defined(IOTA_MAINNET)
#define SNAPSHOT_STATE_FILE_NAME "mainnet.snapshot.state"
#define SNAPSHOT_METADATA_FILE_NAME "mainnet.snapshot.meta"
#elif defined(IOTA_TESTNET)
#define SNAPSHOT_STATE_FILE_NAME "testnet.snapshot.state"
#define SNAPSHOT_METADATA_FILE_NAME "testnet.snapshot.meta"
#else
#error "Unrecognized network: not mainnet nor testnet"
#endif

#define SNAPSHOT_LOGGER_ID "snapshot"

static logger_id_t logger_id;

/*
 * Private functions
 */

retcode_t iota_snapshot_state_read_from_file(snapshot_t *const snapshot, char const *const snapshot_file) {
  retcode_t ret = RC_OK;
  char *buffer = NULL;

  ERR_BIND_GOTO(iota_utils_read_file_into_buffer(snapshot_file, &buffer), ret, cleanup);
  if (buffer) {
    ERR_BIND_GOTO(state_delta_deserialize_str(buffer, &snapshot->state), ret, cleanup);
  }

cleanup:
  if (buffer) {
    free(buffer);
  }

  return ret;
}

retcode_t iota_snapshot_write_to_file(snapshot_t const *const snapshot, char const *const snapshot_file_base) {
  retcode_t ret;
  size_t state_size;
  size_t metadata_size;
  char state_path[FILE_PATH_SIZE];
  char metadata_path[FILE_PATH_SIZE];
  char *buffer = NULL;

  state_size = state_delta_serialized_str_size(snapshot->state);
  metadata_size = iota_snapshot_metadata_serialized_str_size(&snapshot->metadata);

  if ((buffer = (char *)calloc(MAX(state_size, metadata_size), sizeof(char))) == NULL) {
    log_critical(logger_id, "Failed in allocating buffer for snapshot file\n");
    return RC_OOM;
  }

  strcpy(state_path, snapshot_file_base);
  strcat(state_path, IOTA_UTILS_FILE_SEPARATOR);
  strcat(state_path, SNAPSHOT_STATE_FILE_NAME);

  strcpy(metadata_path, snapshot_file_base);
  strcat(metadata_path, IOTA_UTILS_FILE_SEPARATOR);
  strcat(metadata_path, SNAPSHOT_METADATA_FILE_NAME);

  ERR_BIND_GOTO(state_delta_serialize_str(snapshot->state, buffer), ret, cleanup);
  ERR_BIND_GOTO(iota_utils_overwrite_file(state_path, buffer), ret, cleanup);

  ERR_BIND_GOTO(iota_snapshot_metadata_serialize_str(&snapshot->metadata, buffer), ret, cleanup);
  ERR_BIND_GOTO(iota_utils_overwrite_file(metadata_path, buffer), ret, cleanup);

cleanup:
  if (buffer) {
    free(buffer);
  }

  if (ret) {
    log_critical(logger_id, "Failed in writing snapshot file with error code: %s\n", ret);
  }

  return ret;
}

/*
 * Public functions
 */

retcode_t iota_snapshot_init(snapshot_t *const snapshot, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  ERR_BIND_RETURN(iota_snapshot_reset(snapshot, conf), ret);

  if (!snapshot->conf->local_snapshots.local_snapshots_is_enabled ||
      ((ret = iota_snapshot_load_local_snapshot(snapshot, conf)) != RC_OK)) {
    if (ret) {
      iota_snapshot_destroy(snapshot);
    }
    ERR_BIND_RETURN(iota_snapshot_load_built_in_snapshot(snapshot, conf), ret);
  }

  return ret;
}

retcode_t iota_snapshot_reset(snapshot_t *const snapshot, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(SNAPSHOT_LOGGER_ID, LOGGER_DEBUG, true);
  rw_lock_handle_init(&snapshot->rw_lock);
  snapshot->conf = conf;
  snapshot->state = NULL;
  iota_snapshot_metadata_reset(&snapshot->metadata);

  return ret;
}

retcode_t iota_snapshot_load_built_in_snapshot(snapshot_t *const snapshot, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  hash_to_uint64_t_map_t solid_entry_points = NULL;

  hash_to_uint64_t_map_add(&solid_entry_points, conf->genesis_hash, conf->last_milestone);
  iota_snapshot_metadata_init(&snapshot->metadata, conf->genesis_hash, conf->last_milestone,
                              conf->snapshot_timestamp_sec, solid_entry_points);

#if defined(IOTA_MAINNET)
  if (!snapshot->conf->snapshot_signature_skip_validation) {
    bool valid = false;
    if ((ret = iota_file_signature_validate(
             conf->snapshot_file, conf->snapshot_signature_file, snapshot->conf->snapshot_signature_pubkey,
             snapshot->conf->snapshot_signature_depth, snapshot->conf->snapshot_signature_index, &valid)) != RC_OK) {
      log_critical(logger_id, "Validating snapshot signature failed\n");
      return ret;
    } else if (!valid) {
      log_critical(logger_id, "Invalid snapshot signature\n");
      return RC_SNAPSHOT_INVALID_SIGNATURE;
    }
  }
#endif

  if ((ret = iota_snapshot_state_read_from_file(snapshot, conf->snapshot_file))) {
    log_critical(logger_id, "Initializing snapshot initial state failed\n");
    goto cleanup;
  }

  log_info(logger_id, "Consistent snapshot with %ld addresses and correct supply\n", HASH_COUNT(snapshot->state));

cleanup:

  hash_to_uint64_t_map_free(&solid_entry_points);

  return ret;
}

retcode_t iota_snapshot_load_local_snapshot(snapshot_t *const snapshot, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;
  char file_path[256];

  strcpy(file_path, conf->local_snapshots.base_dir);
  strcat(file_path, IOTA_UTILS_FILE_SEPARATOR);
  strcat(file_path, SNAPSHOT_METADATA_FILE_NAME);

  if ((ret = (iota_snapshot_metadata_read_from_file(&snapshot->metadata, file_path)) != RC_OK)) {
    log_critical(logger_id, "Initializing snapshot metadata failed, (file path: %s)\n", file_path);
    return ret;
  }

  strcpy(file_path, conf->local_snapshots.base_dir);
  strcat(file_path, IOTA_UTILS_FILE_SEPARATOR);
  strcat(file_path, SNAPSHOT_STATE_FILE_NAME);

  if ((ret = iota_snapshot_state_read_from_file(snapshot, file_path))) {
    log_critical(logger_id, "Initializing snapshot initial state failed\n");
    return ret;
  }

  log_info(logger_id, "Consistent local snapshot with %ld addresses and correct supply\n", HASH_COUNT(snapshot->state));

  return ret;
}

retcode_t iota_snapshot_destroy(snapshot_t *const snapshot) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  state_delta_destroy(&snapshot->state);
  rw_lock_handle_destroy(&snapshot->rw_lock);

  ERR_BIND_RETURN(iota_snapshot_metadata_destroy(&snapshot->metadata), ret);
  logger_helper_release(logger_id);

  return ret;
}

uint64_t iota_snapshot_get_index(snapshot_t *const snapshot) {
  uint64_t index;

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  index = snapshot->metadata.index;
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return index;
}

retcode_t iota_snapshot_get_balance(snapshot_t *const snapshot, flex_trit_t *const hash, int64_t *balance) {
  retcode_t ret = RC_OK;
  state_delta_entry_t *entry = NULL;

  if (snapshot == NULL || hash == NULL || balance == NULL) {
    return RC_NULL_PARAM;
  }

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  state_delta_find(snapshot->state, hash, entry);
  if (entry == NULL) {
    ret = RC_SNAPSHOT_BALANCE_NOT_FOUND;
  } else {
    *balance = entry->value;
  }
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return ret;
}

retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot, state_delta_t *const delta,
                                     state_delta_t *const patch) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  HASH_CLEAR(hh, *patch);
  rw_lock_handle_rdlock(&snapshot->rw_lock);
  ret = state_delta_create_patch(&snapshot->state, delta, patch);
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return ret;
}

retcode_t iota_snapshot_apply_patch(snapshot_t *const snapshot, state_delta_t *const patch, uint64_t index) {
  retcode_t ret = RC_OK;
  int64_t sum = 0;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  if ((sum = state_delta_sum(patch)) != 0) {
    log_warning(logger_id, "Inconsistent snapshot patch\n");
    return RC_SNAPSHOT_INCONSISTENT_PATCH;
  }

  rw_lock_handle_wrlock(&snapshot->rw_lock);
  ret = iota_snapshot_apply_patch_no_lock(snapshot, patch, index);
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return ret;
}

retcode_t iota_snapshot_apply_patch_no_lock(snapshot_t *const snapshot, state_delta_t *const patch, uint64_t index) {
  snapshot->metadata.index = index;
  return state_delta_apply_patch(&snapshot->state, patch);
}

retcode_t iota_snapshot_copy(snapshot_t const *const src, snapshot_t *const dst) {
  retcode_t ret;

  if (dst == NULL || src == NULL) {
    return RC_NULL_PARAM;
  }

  dst->conf = src->conf;

  ERR_BIND_GOTO(iota_snapshot_metadata_destroy(&dst->metadata), ret, cleanup);
  state_delta_destroy(&dst->state);
  ERR_BIND_GOTO(state_delta_copy(&src->state, &dst->state), ret, cleanup);
  ERR_BIND_GOTO(iota_snapshot_metadata_init(&dst->metadata, src->metadata.hash, src->metadata.index,
                                            src->metadata.timestamp, src->metadata.solid_entry_points),
                ret, cleanup);

cleanup:
  if (ret != RC_OK && dst->state) {
    state_delta_destroy(&dst->state);
  }

  return RC_OK;
}

void iota_snapshot_solid_entry_points_set(snapshot_t *const snapshot, hash243_set_t *const keys) {
  rw_lock_handle_rdlock(&snapshot->rw_lock);
  hash_to_uint64_t_map_keys(&snapshot->metadata.solid_entry_points, keys);
  rw_lock_handle_unlock(&snapshot->rw_lock);
}

bool iota_snapshot_has_solid_entry_point(snapshot_t *const snapshot, flex_trit_t const *const hash) {
  bool is_solid_entry_point;

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  is_solid_entry_point = hash_to_uint64_t_map_contains(snapshot->metadata.solid_entry_points, hash);
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return is_solid_entry_point;
}
