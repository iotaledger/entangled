/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_balances.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_balances_serialize_request(
    serializer_t const *const s, get_balances_req_t const *const obj,
    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("getBalances"));

  ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  cJSON_AddItemToObject(json_root, "threshold",
                        cJSON_CreateNumber(obj->threshold));
  if (obj->tips) {
    ret = hash243_queue_to_json_array(obj->tips, json_root, "tips");
    if (ret != RC_OK) {
      goto err;
    }
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    len = strlen(json_text);
    ret = char_buffer_allocate(out, len);
    if (ret == RC_OK) {
      strncpy(out->data, json_text, len);
    }
  }

  return ret;

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_balances_deserialize_request(serializer_t const *const s,
                                                char const *const obj,
                                                get_balances_req_t *const req) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_uint8(json_obj, "threshold", &req->threshold);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "addresses", &req->addresses);
  if (ret) {
    goto end;
  }
  if (cJSON_GetObjectItemCaseSensitive(json_obj, "tips")) {
    ret = json_array_to_hash243_queue(json_obj, "tips", &req->tips);
  }

end:

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_balances_serialize_response(
    serializer_t const *const s, const get_balances_res_t *const res,
    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = utarray_uint64_to_json_array(res->balances, json_root, "balances");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(res->milestone, json_root, "references");
  if (ret != RC_OK) {
    goto err;
  }

  cJSON_AddItemToObject(json_root, "milestoneIndex",
                        cJSON_CreateNumber(res->milestoneIndex));

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    len = strlen(json_text);
    ret = char_buffer_allocate(out, len);
    if (ret == RC_OK) {
      strncpy(out->data, json_text, len);
    }
  }

  return ret;

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_balances_deserialize_response(serializer_t const *const s,
                                                 char const *const obj,
                                                 get_balances_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_uint64(json_obj, "balances", out->balances);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "references", &out->milestone);
  if (ret) {
    goto end;
  }

  ret = json_get_int(json_obj, "milestoneIndex", &out->milestoneIndex);
  if (ret != RC_OK) {
    goto end;
  }

end:

  cJSON_Delete(json_obj);
  return ret;
}
