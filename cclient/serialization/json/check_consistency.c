/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/check_consistency.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static const char *kCmdName = "checkConsistency";
static const char *kTails = "tails";
static const char *kInfo = "info";
static const char *kState = "state";

retcode_t json_check_consistency_serialize_request(const serializer_t *const s, check_consistency_req_t *const obj,
                                                   char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString(kCmdName));

  ret = hash243_queue_to_json_array(obj->tails, json_root, kTails);
  if (ret != RC_OK) {
    cJSON_Delete(json_root);
    return ret;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    len = strlen(json_text);
    ret = char_buffer_allocate(out, len);
    if (ret == RC_OK) {
      strncpy(out->data, json_text, len);
    }
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_check_consistency_serialize_response(const serializer_t *const s, check_consistency_res_t *const obj,
                                                    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddBoolToObject(json_root, kState, obj->state);

  if (!obj->state) {
    cJSON_AddStringToObject(json_root, kInfo, obj->info->data);
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    len = strlen(json_text);
    ret = char_buffer_allocate(out, len);
    if (ret == RC_OK) {
      strncpy(out->data, json_text, len);
    }
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_check_consistency_deserialize_request(const serializer_t *const s, const char *const obj,
                                                     check_consistency_req_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, kTails, &out->tails);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_check_consistency_deserialize_response(const serializer_t *const s, const char *const obj,
                                                      check_consistency_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "state");
  if (!json_item) {
    ret = RC_CCLIENT_JSON_PARSE;
    goto done;
  }

  out->state = cJSON_IsTrue(json_item);

  if (!out->state) {
    if (!out->info) {
      out->info = char_buffer_new();
    }
    ret = json_get_string(json_obj, "info", out->info);
  }

done:
  cJSON_Delete(json_obj);
  return ret;
}
