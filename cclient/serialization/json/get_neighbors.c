/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_neighbors.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_neighbors_serialize_request(serializer_t const *const s,
                                               char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *req_text = "{\"command\":\"getNeighbors\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_neighbors_serialize_response(
    serializer_t const *const s, get_neighbors_res_t const *const obj,
    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len;

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = neighbor_info_utarray_to_json_array(obj, json_root, "neighbors");
  if (ret) {
    goto err;
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

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_neighbors_deserialize_response(serializer_t const *const s,
                                                  char const *const obj,
                                                  get_neighbors_res_t *out) {
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

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "neighbors");
  if (cJSON_IsArray(json_item)) {
    cJSON *current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      char_buffer_t *addr = char_buffer_new();
      int all_trans, invalid_trans, new_trans;
      ret = json_get_string(current_obj, "address", addr);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfAllTransactions", &all_trans);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfInvalidTransactions",
                         &invalid_trans);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfNewTransactions", &new_trans);
      if (ret != RC_OK) {
        goto end;
      }

      ret = get_neighbors_res_add_neighbor(out, addr, all_trans, invalid_trans,
                                           new_trans);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
