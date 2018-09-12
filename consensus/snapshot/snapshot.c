/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/snapshot/snapshot.h"
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
      memcpy(entry->hash, line, 81);  // TODO
      entry->value = value;
      HASH_ADD(hh, snapshot->state, hash, sizeof(entry->hash), entry);
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

retcode_t iota_snapshot_init(snapshot_t *const snapshot, bool testnet) {
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
  // TODO snapshot file
  if ((ret = iota_snapshot_initial_state(
           snapshot,
           "/Users/thibault/iota/entangled/ciri/resources/"
           "snapshot_testnet.txt"))) {
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
  state_entry_t *entry, *tmp;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  HASH_ITER(hh, snapshot->state, entry, tmp) {
    HASH_DEL(snapshot->state, entry);
    free(entry);
  }
  rw_lock_handle_destroy(&snapshot->rw_lock);
  logger_helper_destroy(SNAPSHOT_LOGGER_ID);
  return RC_OK;
}
