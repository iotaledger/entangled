/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "cJSON.h"

#include "common/model/transaction.h"
#include "consensus/conf.h"
#include "consensus/snapshot/snapshot.h"
#include "utils/logger_helper.h"
#include "utils/signed_files.h"

#define SNAPSHOT_LOGGER_ID "consensus_snapshot"

/*
 * Private functions
 */

static retcode_t iota_snapshot_initial_state(snapshot_t *const snapshot,
                                             char const *const snapshot_file) {
  retcode_t ret = RC_OK;
  char *line = NULL, *delim = NULL;
  int64_t value = 0, supply = 0;
  size_t len = 0;
  ssize_t rd = 0;
  FILE *fp = NULL;
  flex_trit_t hash[FLEX_TRIT_SIZE_243];

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
      flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t *)line,
                             NUM_TRYTES_HASH, NUM_TRYTES_HASH);
      if ((ret = state_delta_add(&snapshot->state, hash, value)) != RC_OK) {
        goto done;
      }
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

retcode_t iota_snapshot_init_conf(iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;
  FILE *file = NULL;
  size_t len = 0;
  char *content = NULL;
  cJSON *json = NULL, *tmp = NULL, *timestamp = NULL, *signature = NULL,
        *coordinator = NULL;

  if ((file = fopen(conf->snapshot_conf_file, "r")) == NULL) {
    log_error(SNAPSHOT_LOGGER_ID, "Snapshot configuration file not found\n");
    ret = RC_SNAPSHOT_FILE_NOT_FOUND;
    goto done;
  }

  if (fseek(file, 0, SEEK_END) < 0 || (len = ftell(file)) <= 0 ||
      fseek(file, 0, SEEK_SET) < 0) {
    log_error(SNAPSHOT_LOGGER_ID, "Invalid snapshot configuration file\n");
    ret = RC_SNAPSHOT_INVALID_FILE;
    goto done;
  }

  if ((content = (char *)malloc(len + 1)) == NULL) {
    ret = RC_SNAPSHOT_OOM;
    goto done;
  }

  if (fread(content, 1, len, file) < 0) {
    log_error(SNAPSHOT_LOGGER_ID, "Invalid snapshot configuration file\n");
    ret = RC_SNAPSHOT_INVALID_FILE;
    goto done;
  }

  content[len] = '\0';

  if ((json = cJSON_Parse(content)) == NULL) {
    goto json_error;
  }

  if ((timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp")) ==
          NULL ||
      (coordinator = cJSON_GetObjectItemCaseSensitive(json, "coordinator")) ==
          NULL) {
    goto json_error;
  }

  if (!cJSON_IsNumber(timestamp)) {
    goto json_error;
  }
  conf->snapshot_timestamp_sec = timestamp->valueint;

  tmp = cJSON_GetObjectItemCaseSensitive(coordinator, "pubkey");
  if (tmp == NULL || !cJSON_IsString(tmp) || tmp->valuestring == NULL ||
      strlen(tmp->valuestring) != HASH_LENGTH_TRYTE) {
    goto json_error;
  }
  flex_trits_from_trytes(conf->coordinator, HASH_LENGTH_TRIT,
                         (tryte_t *)tmp->valuestring, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);

  tmp = cJSON_GetObjectItemCaseSensitive(coordinator, "lastMilestone");
  if (tmp == NULL || !cJSON_IsNumber(tmp)) {
    goto json_error;
  }
  conf->last_milestone = tmp->valueint;

  if ((signature = cJSON_GetObjectItemCaseSensitive(json, "signature")) !=
      NULL) {
    tmp = cJSON_GetObjectItemCaseSensitive(signature, "index");
    if (tmp == NULL || !cJSON_IsNumber(tmp)) {
      goto json_error;
    }
    conf->snapshot_signature_index = tmp->valueint;

    tmp = cJSON_GetObjectItemCaseSensitive(signature, "depth");
    if (tmp == NULL || !cJSON_IsNumber(tmp)) {
      goto json_error;
    }
    conf->snapshot_signature_depth = tmp->valueint;

    tmp = cJSON_GetObjectItemCaseSensitive(signature, "pubkey");
    if (tmp == NULL || !cJSON_IsString(tmp) || tmp->valuestring == NULL ||
        strlen(tmp->valuestring) != HASH_LENGTH_TRYTE) {
      goto json_error;
    }
    flex_trits_from_trytes(conf->snapshot_signature_pubkey, HASH_LENGTH_TRIT,
                           (tryte_t *)tmp->valuestring, HASH_LENGTH_TRYTE,
                           HASH_LENGTH_TRYTE);
  }

  goto done;

json_error : {
  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr != NULL) {
    log_error(SNAPSHOT_LOGGER_ID, "%s\n", error_ptr);
  }
  ret = RC_SNAPSHOT_FAILED_JSON_PARSING;
}

done:
  if (file) {
    fclose(file);
  }
  if (content) {
    free(content);
  }
  if (json) {
    cJSON_Delete(json);
  }
  return ret;
}

retcode_t iota_snapshot_init(snapshot_t *const snapshot,
                             iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  logger_helper_init(SNAPSHOT_LOGGER_ID, LOGGER_DEBUG, true);
  rw_lock_handle_init(&snapshot->rw_lock);
  snapshot->conf = conf;
  snapshot->index = 0;
  snapshot->state = NULL;

  if ((ret = iota_snapshot_init_conf(snapshot->conf)) != RC_OK) {
    log_critical(SNAPSHOT_LOGGER_ID,
                 "Parsing snapshot configuration file failed\n");
    return ret;
  }

  if (strlen(snapshot->conf->snapshot_sig_file)) {
    bool valid = false;
    if ((ret = iota_file_signature_validate(
             conf->snapshot_file, conf->snapshot_sig_file,
             snapshot->conf->snapshot_signature_pubkey,
             snapshot->conf->snapshot_signature_depth,
             snapshot->conf->snapshot_signature_index, &valid)) != RC_OK) {
      log_critical(SNAPSHOT_LOGGER_ID,
                   "Validating snapshot signature failed\n");
      return ret;
    } else if (!valid) {
      log_critical(SNAPSHOT_LOGGER_ID, "Invalid snapshot signature\n");
      return RC_SNAPSHOT_INVALID_SIGNATURE;
    }
  }
  if ((ret = iota_snapshot_initial_state(snapshot, conf->snapshot_file))) {
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

  state_delta_destroy(&snapshot->state);
  rw_lock_handle_destroy(&snapshot->rw_lock);
  logger_helper_destroy(SNAPSHOT_LOGGER_ID);
  return ret;
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
  state_delta_entry_t *entry = NULL;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  } else if (hash == NULL) {
    return RC_SNAPSHOT_NULL_HASH;
  } else if (balance == NULL) {
    return RC_SNAPSHOT_NULL_BALANCE;
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

retcode_t iota_snapshot_create_patch(snapshot_t *const snapshot,
                                     state_delta_t *const delta,
                                     state_delta_t *const patch) {
  retcode_t ret = RC_OK;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  HASH_CLEAR(hh, *patch);
  rw_lock_handle_rdlock(&snapshot->rw_lock);

  if ((ret = state_delta_create_patch(&snapshot->state, delta, patch)) !=
      RC_OK) {
    goto done;
  }

done:
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return ret;
}

retcode_t iota_snapshot_apply_patch(snapshot_t *const snapshot,
                                    state_delta_t *const patch, size_t index) {
  retcode_t ret = RC_OK;
  int64_t sum = 0;

  if (snapshot == NULL) {
    return RC_SNAPSHOT_NULL_SELF;
  }

  if ((sum = state_delta_sum(patch)) != 0) {
    log_warning(SNAPSHOT_LOGGER_ID, "Inconsistent snapshot patch\n");
    return RC_SNAPSHOT_INCONSISTENT_PATCH;
  }

  rw_lock_handle_wrlock(&snapshot->rw_lock);

  if ((ret = state_delta_apply_patch(&snapshot->state, patch)) != RC_OK) {
    goto done;
  }

  snapshot->index = index;

done:
  rw_lock_handle_unlock(&snapshot->rw_lock);
  return ret;
}
