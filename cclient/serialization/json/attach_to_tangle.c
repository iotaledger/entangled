/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/attach_to_tangle.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static const char *kCmdName = "attachToTangle";
static const char *kTrunk = "trunkTransaction";
static const char *kBranch = "branchTransaction";
static const char *kTrytes = "trytes";
static const char *kMwm = "minWeightMagnitude";

retcode_t json_attach_to_tangle_serialize_request(attach_to_tangle_req_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString(kCmdName));

  ret = flex_trits_to_json_string(json_root, kTrunk, obj->trunk, NUM_TRITS_HASH);
  if (ret != RC_OK) {
    goto done;
  }

  ret = flex_trits_to_json_string(json_root, kBranch, obj->branch, NUM_TRITS_HASH);
  if (ret != RC_OK) {
    goto done;
  }

  cJSON_AddItemToObject(json_root, kMwm, cJSON_CreateNumber(obj->mwm));

  ret = hash8019_array_to_json_array(obj->trytes, json_root, kTrytes);
  if (ret != RC_OK) {
    goto done;
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }
done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_attach_to_tangle_serialize_response(attach_to_tangle_res_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = hash8019_array_to_json_array(obj->trytes, json_root, kTrytes);
  if (ret != RC_OK) {
    goto done;
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_attach_to_tangle_deserialize_request(char const *const obj, attach_to_tangle_req_t *const out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  if (out->trytes == NULL) {
    out->trytes = hash8019_array_new();
  }

  if (out->trytes == NULL) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_OOM));
    return RC_OOM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_uint8(json_obj, kMwm, &out->mwm);
  if (ret != RC_OK) goto done;

  ret = json_string_hash_to_flex_trits(json_obj, kTrunk, out->trunk);
  if (ret != RC_OK) goto done;

  ret = json_string_hash_to_flex_trits(json_obj, kBranch, out->branch);
  if (ret != RC_OK) goto done;

  ret = json_array_to_hash8019_array(json_obj, kTrytes, out->trytes);
  if (ret != RC_OK) goto done;

done:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_attach_to_tangle_deserialize_response(char const *const obj, attach_to_tangle_res_t *const out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash8019_array(json_obj, kTrytes, out->trytes);

  cJSON_Delete(json_obj);
  return ret;
}
