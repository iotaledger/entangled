/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/add_neighbors.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_add_neighbors_serialize_request(add_neighbors_req_t const *const req, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!req || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  if (utarray_len(req->uris) == 0) {
    log_error(json_logger_id, "[%s:%d] URI list is empty\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("addNeighbors"));

  ret = utarray_to_json_array(req->uris, json_root, "uris");
  if (ret != RC_OK) {
    cJSON_Delete(json_root);
    return ret;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_add_neighbors_serialize_response(add_neighbors_res_t const *const res, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!res || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "addedNeighbors", cJSON_CreateNumber(res->added_neighbors));

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_add_neighbors_deserialize_request(char const *const obj, add_neighbors_req_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_string_array_to_utarray(json_obj, "uris", out->uris);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_add_neighbors_deserialize_response(char const *const obj, add_neighbors_res_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_int(json_obj, "addedNeighbors", &out->added_neighbors);

  cJSON_Delete(json_obj);
  return ret;
}
