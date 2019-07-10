/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_trytes.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_trytes_serialize_request(get_trytes_req_t const *const req, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  if (!req->hashes) {
    log_error(json_logger_id, "[%s:%d] Null parameter\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("getTrytes"));

  ret = hash243_queue_to_json_array(req->hashes, json_root, "hashes");
  if (ret != RC_OK) {
    goto done;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_trytes_deserialize_request(char const *const obj, get_trytes_req_t *const req) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "hashes", &req->hashes);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_trytes_serialize_response(get_trytes_res_t const *const res, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = hash8019_queue_to_json_array(res->trytes, json_root, "trytes");
  if (ret != RC_OK) {
    goto done;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_trytes_deserialize_response(char const *const obj, get_trytes_res_t *const res) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash8019_queue(json_obj, "trytes", &res->trytes);
  cJSON_Delete(json_obj);
  return ret;
}
