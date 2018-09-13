/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot.h"
#include "common/model/transaction.h"
#include "utils/logger_helper.h"

#define SNAPSHOT_LOGGER_ID "consensus_snapshot"

/*
 * Private functions
 */

static retcode_t iota_snapshot_initial_state(snapshot_t *const snapshot,
                                             char const *const snapshot_file) {
  retcode_t ret = RC_OK;
  char *line = NULL, *delim = NULL;
  int64_t value = 0, supply = 0;
  state_entry_t *entry = NULL;
  size_t len = 0;
  ssize_t rd = 0;
  FILE *fp = NULL;

  if ((fp = fopen(snapshot_file, "r")) == NULL) {
    log_critical(SNAPSHOT_LOGGER_ID, "Opening snapshot file failed\n");
    ret = RC_SNAPSHOT_FILE_NOT_FOUND;
    goto done;
  }

  while ((rd = getline(&line, &len, fp)) > 0) {
    line[--rd] = '\0';
    if ((delim = strchr(line, ';')) == NULL) {
      log_critical(SNAPSHOT_LOGGER_ID, "Badly formatted snapshot file\n");
      ret = RC_SNAPSHOT_INVALID_FILE;
      goto done;
    }
    *delim = '\0';
    value = atoll(delim + 1);
    if (value > 0) {
      if ((entry = malloc(sizeof(state_entry_t))) == NULL) {
        ret = RC_SNAPSHOT_OOM;
        goto done;
      }
      flex_trits_from_trytes(entry->hash, FLEX_TRIT_SIZE_243, (tryte_t *)line,
                             NUM_TRYTES_HASH, NUM_TRYTES_HASH);
      entry->value = value;
      HASH_ADD(hh, snapshot->state, hash, FLEX_TRIT_SIZE_243, entry);
      supply += value;
    } else if (value < 0) {
      ret = RC_SNAPSHOT_INCONSISTENT_SNAPSHOT;
      log_critical(SNAPSHOT_LOGGER_ID, "Inconsistent snapshot\n");
      goto done;
    }
  }
  if (supply != IOTA_SUPPLY) {
    ret = RC_SNAPSHOT_INVALID_SUPPLY;
    log_critical(SNAPSHOT_LOGGER_ID, "Invalid snapshot supply: %ld\n", supply);
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

/*
 * Public functions
 */

retcode_t iota_snapshot_init(snapshot_t *const snapshot,
                             char const *const snapshot_file,
                             char const *const snapshot_sig_file,
                             bool testnet) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  logger_helper_init(SNAPSHOT_LOGGER_ID, LOGGER_DEBUG, true);
  rw_lock_handle_init(&snapshot->rw_lock);
  snapshot->index = 0;
  snapshot->state = NULL;

  // TODO signature validation
  // String snapshotFile = config.getSnapshotFile();
  // if (!config.isTestnet() &&
  //     !SignedFiles.isFileSignatureValid(
  //         snapshotFile, config.getSnapshotSignatureFile(), SNAPSHOT_PUBKEY,
  //         SNAPSHOT_PUBKEY_DEPTH, SNAPSHOT_INDEX)) {
  //   throw new RuntimeException("Snapshot signature failed.");
  // }
  if ((ret = iota_snapshot_initial_state(snapshot, snapshot_file))) {
    log_critical(SNAPSHOT_LOGGER_ID,
                 "Initializing snapshot initial state failed\n");
    return ret;
  }
  log_info(SNAPSHOT_LOGGER_ID,
           "Consistent snapshot with %ld addresses and correct supply\n",
           HASH_COUNT(snapshot->state));
  return ret;
}

retcode_t iota_snapshot_destroy(snapshot_t *const snapshot) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  ret = iota_snapshot_state_destroy(&snapshot->state);
  rw_lock_handle_destroy(&snapshot->rw_lock);
  logger_helper_destroy(SNAPSHOT_LOGGER_ID);
  return ret;
}

retcode_t iota_snapshot_state_destroy(state_map_t *const state) {
  state_entry_t *entry, *tmp;

  if (state == NULL || *state == NULL) {
    return RC_SNAPSHOT_NULL_STATE;
  }

  HASH_ITER(hh, *state, entry, tmp) {
    HASH_DEL(*state, entry);
    free(entry);
  }
  return RC_OK;
}

bool iota_snapshot_is_state_consistent(state_map_t *const state) {
  state_entry_t *entry, *tmp;

  if (state == NULL) {
    return false;
  }

  HASH_ITER(hh, *state, entry, tmp) {
    if (entry->value <= 0) {
      if (entry->value < 0) {
        return false;
      }
      HASH_DEL(*state, entry);
      free(entry);
    }
  }
  return true;
}

size_t iota_snapshot_get_index(snapshot_t *const snapshot) {
  size_t index;

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  index = snapshot->index;
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return index;
}

retcode_t iota_snapshot_get_balance(snapshot_t *const snapshot,
                                    flex_trit_t *const hash, int64_t *balance) {
  retcode_t ret = RC_OK;
  state_entry_t *entry;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  } else if (hash == NULL) {
    return RC_SNAPSHOT_NULL_HASH;
  } else if (balance == NULL) {
    return RC_SNAPSHOT_NULL_BALANCE;
  }

  rw_lock_handle_rdlock(&snapshot->rw_lock);
  HASH_FIND(hh, snapshot->state, &hash, FLEX_TRIT_SIZE_243, entry);
  if (entry == NULL) {
    ret = RC_SNAPSHOT_BALANCE_NOT_FOUND;
  } else {
    *balance = entry->value;
  }
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return ret;
}

retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot,
                                     state_map_t *const diff,
                                     state_map_t *const patch) {
  state_entry_t *iter, *entry, *new, *tmp;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  HASH_CLEAR(hh, *patch);
  rw_lock_handle_rdlock(&snapshot->rw_lock);
  HASH_ITER(hh, *diff, iter, tmp) {
    HASH_FIND(hh, snapshot->state, &iter->hash, FLEX_TRIT_SIZE_243, entry);
    if ((new = malloc(sizeof(state_entry_t))) == NULL) {
      return RC_SNAPSHOT_OOM;
    }
    memcpy(new->hash, iter->hash, FLEX_TRIT_SIZE_243);
    new->value = (entry ? entry->value : 0) + iter->value;
    HASH_ADD(hh, *patch, hash, FLEX_TRIT_SIZE_243, new);
  }
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return RC_OK;
}

retcode_t iota_snapshot_apply_patch(snapshot_t *const snapshot,
                                    state_map_t *const patch, size_t index) {
  state_entry_t *iter, *entry, *new, *tmp;
  ssize_t sum = 0;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  HASH_ITER(hh, *patch, iter, tmp) { sum += iter->value; }
  if (sum != 0) {
    log_warning(SNAPSHOT_LOGGER_ID, "Inconsistent snapshot patch\n");
    return RC_SNAPSHOT_INCONSISTENT_PATCH;
  }

  rw_lock_handle_wrlock(&snapshot->rw_lock);
  HASH_ITER(hh, *patch, iter, tmp) {
    HASH_FIND(hh, snapshot->state, &iter->hash, FLEX_TRIT_SIZE_243, entry);
    if (entry) {
      entry += iter->value;
    } else {
      if ((new = malloc(sizeof(state_entry_t))) == NULL) {
        return RC_SNAPSHOT_OOM;
      }
      memcpy(new->hash, iter->hash, FLEX_TRIT_SIZE_243);
      new->value = iter->value;
      HASH_ADD(hh, snapshot->state, hash, FLEX_TRIT_SIZE_243, new);
    }
  }
  snapshot->index = index;
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return RC_OK;
}
