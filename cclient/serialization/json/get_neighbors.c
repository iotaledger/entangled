/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_neighbors.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static retcode_t neighbor_info_utarray_to_json_array(UT_array const *const ut, cJSON *const json_root,
                                                     char const *const obj_name) {
  neighbor_info_t *nbr_info = NULL;

  if (!ut || !json_root || !obj_name) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *array_obj = cJSON_CreateArray();
  if (array_obj == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, obj_name, array_obj);

  while ((nbr_info = (neighbor_info_t *)utarray_next(ut, nbr_info))) {
    cJSON *json_nbr_info = cJSON_CreateObject();

    cJSON_AddStringToObject(json_nbr_info, "address", nbr_info->address->data);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfAllTransactions", nbr_info->all_trans_num);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfRandomTransactionRequests", nbr_info->random_trans_req_num);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfNewTransactions", nbr_info->new_trans_num);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfInvalidTransactions", nbr_info->invalid_trans_num);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfStaleTransactions", nbr_info->stale_trans_num);
    cJSON_AddNumberToObject(json_nbr_info, "numberOfSentTransactions", nbr_info->sent_trans_num);
    cJSON_AddStringToObject(json_nbr_info, "connectiontype", nbr_info->connection_type->data);

    cJSON_AddItemToArray(array_obj, json_nbr_info);
  }

  return RC_OK;
}

retcode_t json_get_neighbors_serialize_request(char_buffer_t *out) {
  char const *req_text = "{\"command\":\"getNeighbors\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  return char_buffer_set(out, req_text);
}

retcode_t json_get_neighbors_serialize_response(get_neighbors_res_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = neighbor_info_utarray_to_json_array(obj, json_root, "neighbors");
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

retcode_t json_get_neighbors_deserialize_response(char const *const obj, get_neighbors_res_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_item = NULL;
  cJSON *json_obj = cJSON_Parse(obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  char_buffer_t *addr = char_buffer_new();
  char_buffer_t *connection = char_buffer_new();
  if (!addr || !connection) {
    ret = RC_OOM;
    goto end;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "neighbors");
  if (cJSON_IsArray(json_item)) {
    cJSON *current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      uint32_t all_trans_num, random_trans_req_num, new_trans_num, invalid_trans_num, stale_trans_num, sent_trans_num;
      ret = json_get_string(current_obj, "address", addr);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfAllTransactions", &all_trans_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfRandomTransactionRequests", &random_trans_req_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfNewTransactions", &new_trans_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfInvalidTransactions", &invalid_trans_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfStaleTransactions", &stale_trans_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_uint32(current_obj, "numberOfSentTransactions", &sent_trans_num);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_string(current_obj, "connectiontype", connection);
      if (ret != RC_OK) {
        goto end;
      }

      ret = get_neighbors_res_add_neighbor(out, addr->data, all_trans_num, random_trans_req_num, new_trans_num,
                                           invalid_trans_num, stale_trans_num, sent_trans_num, connection->data);
    }
  }

end:
  cJSON_Delete(json_obj);
  char_buffer_free(addr);
  char_buffer_free(connection);
  return ret;
}
