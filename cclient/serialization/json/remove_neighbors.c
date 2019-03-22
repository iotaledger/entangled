/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/remove_neighbors.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_remove_neighbors_serialize_request(serializer_t const *const s, remove_neighbors_req_t const *const req,
                                                  char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("removeNeighbors"));

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

retcode_t json_remove_neighbors_deserialize_request(serializer_t const *const s, char const *const obj,
                                                    remove_neighbors_req_t *req) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_string_array_to_utarray(json_obj, "uris", req->uris);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_remove_neighbors_serialize_response(serializer_t const *const s, remove_neighbors_res_t const *const res,
                                                   char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddNumberToObject(json_root, "removedNeighbors", res->removed_neighbors);

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_remove_neighbors_deserialize_response(serializer_t const *const s, char const *const obj,
                                                     remove_neighbors_res_t *res) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_int(json_obj, "removedNeighbors", &res->removed_neighbors);

  cJSON_Delete(json_obj);
  return ret;
}
