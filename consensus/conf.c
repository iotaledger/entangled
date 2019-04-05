/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <stdlib.h>

#include "cJSON.h"

#include "consensus/conf.h"
#include "utils/logger_helper.h"

#define CONSENSUS_CONF_LOGGER_ID "consensus_conf"

static logger_id_t logger_id;

retcode_t iota_snapshot_conf_init(iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;
  FILE *file = NULL;
  size_t len = 0;
  char *content = NULL;
  cJSON *json = NULL, *tmp = NULL, *timestamp = NULL, *signature = NULL, *coordinator = NULL;

  if ((file = fopen(conf->snapshot_conf_file, "r")) == NULL) {
    goto done;
  }

  if (fseek(file, 0, SEEK_END) < 0 || (len = ftell(file)) <= 0 || fseek(file, 0, SEEK_SET) < 0) {
    log_error(logger_id, "Invalid snapshot configuration file\n");
    ret = RC_SNAPSHOT_INVALID_FILE;
    goto done;
  }

  if ((content = (char *)malloc(len + 1)) == NULL) {
    ret = RC_SNAPSHOT_OOM;
    goto done;
  }

  if (fread(content, 1, len, file) < 0) {
    log_error(logger_id, "Invalid snapshot configuration file\n");
    ret = RC_SNAPSHOT_INVALID_FILE;
    goto done;
  }

  content[len] = '\0';

  if ((json = cJSON_Parse(content)) == NULL) {
    goto json_error;
  }

  if ((timestamp = cJSON_GetObjectItemCaseSensitive(json, "timestamp")) == NULL ||
      (coordinator = cJSON_GetObjectItemCaseSensitive(json, "coordinator")) == NULL) {
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
  flex_trits_from_trytes(conf->coordinator_address, HASH_LENGTH_TRIT, (tryte_t *)tmp->valuestring, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);

  tmp = cJSON_GetObjectItemCaseSensitive(coordinator, "lastMilestone");
  if (tmp == NULL || !cJSON_IsNumber(tmp)) {
    goto json_error;
  }
  conf->last_milestone = tmp->valueint;

  if ((signature = cJSON_GetObjectItemCaseSensitive(json, "signature")) != NULL) {
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
    flex_trits_from_trytes(conf->snapshot_signature_pubkey, HASH_LENGTH_TRIT, (tryte_t *)tmp->valuestring,
                           HASH_LENGTH_TRYTE, HASH_LENGTH_TRYTE);
  }

  goto done;

json_error : {
  const char *error_ptr = cJSON_GetErrorPtr();
  if (error_ptr != NULL) {
    log_error(logger_id, "%s\n", error_ptr);
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

retcode_t iota_consensus_conf_init(iota_consensus_conf_t *const conf) {
  retcode_t ret = RC_OK;

  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  logger_id = logger_helper_enable(CONSENSUS_CONF_LOGGER_ID, LOGGER_DEBUG, true);

  conf->alpha = DEFAULT_TIP_SELECTION_ALPHA;
  conf->below_max_depth = DEFAULT_TIP_SELECTION_BELOW_MAX_DEPTH;
  flex_trits_from_trytes(conf->coordinator_address, HASH_LENGTH_TRIT, (tryte_t *)COORDINATOR_ADDRESS, HASH_LENGTH_TRYTE,
                         HASH_LENGTH_TRYTE);
  conf->coordinator_depth = DEFAULT_COORDINATOR_DEPTH;
  conf->coordinator_security_level = DEFAULT_COORDINATOR_SECURITY_LEVEL;
  conf->coordinator_signature_type = DEFAULT_COORDINATOR_SIGNATURE_TYPE;
  memset(conf->genesis_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  conf->max_depth = DEFAULT_TIP_SELECTION_MAX_DEPTH;
  conf->mwm = DEFAULT_MWN;
  strcpy(conf->snapshot_conf_file, DEFAULT_SNAPSHOT_CONF_FILE);
  strcpy(conf->snapshot_file, DEFAULT_SNAPSHOT_FILE);
  strcpy(conf->snapshot_signature_file, DEFAULT_SNAPSHOT_SIG_FILE);
  conf->snapshot_signature_skip_validation = DEFAULT_SNAPSHOT_SIGNATURE_SKIP_VALIDATION;

  ret = iota_snapshot_conf_init(conf);

  conf->coordinator_max_milestone_index = 1 << conf->coordinator_depth;

  logger_helper_release(logger_id);

  return ret;
}
