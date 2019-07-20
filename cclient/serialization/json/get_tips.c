/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_tips.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_tips_serialize_request(char_buffer_t *out) {
  char const *req_text = "{\"command\":\"getTips\"}";
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }
  return char_buffer_set(out, req_text);
}

retcode_t json_get_tips_serialize_response(get_tips_res_t const *const res, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;

  if (!res || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = hash243_stack_to_json_array(res->hashes, json_root, "hashes");
  if (ret) {
    goto err;
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_tips_deserialize_response(char const *const obj, get_tips_res_t *res) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!res || !obj) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_stack(json_obj, "hashes", &res->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
