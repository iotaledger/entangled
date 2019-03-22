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
  if (!ut) {
    return RC_NULL_PARAM;
  }
  if (utarray_len(ut) > 0) {
    cJSON *array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
      return RC_CCLIENT_JSON_CREATE;
    }

    cJSON_AddItemToObject(json_root, obj_name, array_obj);
    neighbor_info_t *nbr_info = NULL;
    while ((nbr_info = (neighbor_info_t *)utarray_next(ut, nbr_info))) {
      cJSON *json_nbr_info = cJSON_CreateObject();

      cJSON_AddStringToObject(json_nbr_info, "address", nbr_info->address->data);
      cJSON_AddNumberToObject(json_nbr_info, "numberOfAllTransactions", nbr_info->all_trans_num);
      cJSON_AddNumberToObject(json_nbr_info, "numberOfInvalidTransactions", nbr_info->invalid_trans_num);
      cJSON_AddNumberToObject(json_nbr_info, "numberOfNewTransactions", nbr_info->new_trans_num);

      cJSON_AddItemToArray(array_obj, json_nbr_info);
    }
  }
  return RC_OK;
}

retcode_t json_get_neighbors_serialize_request(serializer_t const *const s, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *req_text = "{\"command\":\"getNeighbors\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  ret = char_buffer_set(out, req_text);
  return ret;
}

retcode_t json_get_neighbors_serialize_response(serializer_t const *const s, get_neighbors_res_t const *const obj,
                                                char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = neighbor_info_utarray_to_json_array(obj, json_root, "neighbors");
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

retcode_t json_get_neighbors_deserialize_response(serializer_t const *const s, char const *const obj,
                                                  get_neighbors_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

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
      ret = json_get_int(current_obj, "numberOfInvalidTransactions", &invalid_trans);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfNewTransactions", &new_trans);
      if (ret != RC_OK) {
        goto end;
      }

      ret = get_neighbors_res_add_neighbor(out, addr, all_trans, invalid_trans, new_trans);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
