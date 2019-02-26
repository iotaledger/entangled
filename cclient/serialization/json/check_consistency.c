/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/check_consistency.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_check_consistency_serialize_request(
    const serializer_t* const s, check_consistency_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("checkConsistency"));

  ret = hash243_queue_to_json_array(obj->tails, json_root, "tails");
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
    cJSON_free((void*)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_check_consistency_deserialize_response(
    const serializer_t* const s, const char* const obj,
    check_consistency_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  if (json_obj == NULL) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(json_logger_id, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "state");
  out->state = cJSON_IsTrue(json_item);
  ret = json_get_string(json_obj, "info", out->info);

  cJSON_Delete(json_obj);
  return ret;
}
