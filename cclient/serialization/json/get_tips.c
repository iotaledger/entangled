/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_tips.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_tips_serialize_request(serializer_t const *const s,
                                          char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *req_text = "{\"command\":\"getTips\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_tips_serialize_response(serializer_t const *const s,
                                           get_tips_res_t const *const res,
                                           char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = hash243_stack_to_json_array(res->hashes, json_root, "hashes");
  if (ret) {
    goto err;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_tips_deserialize_response(serializer_t const *const s,
                                             char const *const obj,
                                             get_tips_res_t *res) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
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

  ret = json_array_to_hash243_stack(json_obj, "hashes", &res->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
