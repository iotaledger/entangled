/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_balances.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_balances_serialize_request(get_balances_req_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  if (!obj->addresses) {
    log_error(json_logger_id, "[%s:%d] The addresses parameter is needed\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("getBalances"));

  ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  cJSON_AddItemToObject(json_root, "threshold", cJSON_CreateNumber(obj->threshold));
  if (obj->tips) {
    ret = hash243_queue_to_json_array(obj->tips, json_root, "tips");
    if (ret != RC_OK) {
      goto err;
    }
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

retcode_t json_get_balances_deserialize_request(char const *const obj, get_balances_req_t *const req) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !req) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_uint8(json_obj, "threshold", &req->threshold);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "addresses", &req->addresses);
  if (ret) {
    goto end;
  }

  if (cJSON_HasObjectItem(json_obj, "tips")) {
    if ((ret = json_array_to_hash243_queue(json_obj, "tips", &req->tips)) != RC_OK) {
      goto end;
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_balances_serialize_response(get_balances_res_t const *const res, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
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

  ret = uint64_utarray_to_json_array(res->balances, json_root, "balances");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(res->references, json_root, "references");
  if (ret != RC_OK) {
    goto err;
  }

  cJSON_AddItemToObject(json_root, "milestoneIndex", cJSON_CreateNumber(res->milestone_index));

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_balances_deserialize_response(char const *const obj, get_balances_res_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_uint64(json_obj, "balances", out->balances);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "references", &out->references);
  if (ret) {
    goto end;
  }

  ret = json_get_uint64(json_obj, "milestoneIndex", &out->milestone_index);

end:
  cJSON_Delete(json_obj);
  return ret;
}
