/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot.h"
#include <stdlib.h>
#include "common/model/transaction.h"
#include "consensus/conf.h"
#include "utils/logger_helper.h"
#include "utils/signed_files.h"

#define SNAPSHOT_LOGGER_ID "snapshot"

static logger_id_t logger_id;

/*
 * Private functions
 */

retcode_t iota_snapshot_read_from_file(snapshot_t *const snapshot, char const *const snapshot_file) {
  retcode_t ret = RC_OK;
  char *line = NULL, *delim = NULL;
  int64_t value = 0, supply = 0;
  size_t len = 0;
  ssize_t rd = 0;
  FILE *fp = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

  if ((fp = fopen(snapshot_file, "r")) == NULL) {
    log_critical(logger_id, "Opening snapshot file failed\n");
    ret = RC_SNAPSHOT_FILE_NOT_FOUND;
    goto done;
  }

  while ((rd = getline(&line, &len, fp)) > 0) {
    line[--rd] = '\0';
    if ((delim = strchr(line, ';')) == NULL) {
      log_critical(logger_id, "Badly formatted snapshot file\n");
      ret = RC_SNAPSHOT_INVALID_FILE;
      goto done;
    }
    *delim = '\0';
    value = atoll(delim + 1);
    if (value > 0) {
      flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t *)line, NUM_TRYTES_HASH, NUM_TRYTES_HASH);
      if ((ret = state_delta_add(&snapshot->state, hash, value)) != RC_OK) {
        goto done;
      }
      supply += value;
    } else if (value < 0) {
      ret = RC_SNAPSHOT_INCONSISTENT_SNAPSHOT;
      log_critical(logger_id, "Inconsistent snapshot\n");
      goto done;
    }
  }
  if (supply != IOTA_SUPPLY) {
    ret = RC_SNAPSHOT_INVALID_SUPPLY;
    log_critical(logger_id, "Invalid snapshot supply: %ld\n", supply);
    goto done;
  }

done:
  if (line) {
    free(line);
  }
  if (fp) {
    fclose(fp);
  }

  return ret;
}

retcode_t iota_snapshot_write_to_file(snapshot_t const *const snapshot, char const *const snapshot_file) {
  retcode_t ret;
  size_t size;
  size = state_delta_serialized_str_size(snapshot->state);
  char *buffer;
  if ((buffer = (byte_t *)calloc(size, sizeof(byte_t))) == NULL) {
    ret = RC_STORAGE_OOM;
    goto cleanup;
  }

  ERR_BIND_GOTO(state_delta_serialize_str(snapshot->state, buffer), ret, cleanup);

cleanup:
  return ret;
}

/*
 * Public functions
 */

retcode_t iota_snapshot_init(snapshot_t *const snapshot, iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(SNAPSHOT_LOGGER_ID, LOGGER_DEBUG, true);
  rw_lock_handle_init(&snapshot->rw_lock);
  snapshot->conf = conf;
  snapshot->index = 0;
  snapshot->state = NULL;

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

  if ((ret = iota_snapshot_read_from_file(snapshot, conf->snapshot_file))) {
    log_critical(logger_id, "Initializing snapshot initial state failed\n");
    return ret;
  }
  log_info(logger_id, "Consistent snapshot with %ld addresses and correct supply\n", HASH_COUNT(snapshot->state));

  return ret;
}

retcode_t iota_snapshot_destroy(snapshot_t *const snapshot) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_NULL_PARAM;
  }

  state_delta_destroy(&snapshot->state);
  rw_lock_handle_destroy(&snapshot->rw_lock);
  logger_helper_release(logger_id);

  return ret;
}

uint64_t iota_snapshot_get_index(snapshot_t *const snapshot) {
  uint64_t index;

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  index = snapshot->index;
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
  ret = state_delta_apply_patch(&snapshot->state, patch);
  snapshot->index = index;
  rw_lock_handle_unlock(&snapshot->rw_lock);

  return ret;
}

retcode_t iota_snapshot_copy(snapshot_t const *const src, snapshot_t *const dst) {
  retcode_t ret;
  dst->index = src->index;
  dst->conf = src->conf;

  if (dst == NULL || src == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  rw_lock_handle_init(&dst->rw_lock);
  dst->state = NULL;

  ERR_BIND_RETURN(state_delta_copy(&src->state, &dst->state), ret);
  return RC_OK;
}

void iota_snapshot_lock_write(snapshot_t *const snapshot) { rw_lock_handle_wrlock(&snapshot->rw_lock); }

void iota_snapshot_lock_read(snapshot_t *const snapshot) { rw_lock_handle_rdlock(&snapshot->rw_lock); }

void iota_snapshot_unlock(snapshot_t *const snapshot) { rw_lock_handle_unlock(&snapshot->rw_lock); }