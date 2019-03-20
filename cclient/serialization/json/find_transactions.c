/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/find_transactions.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_find_transactions_serialize_request(serializer_t const *const s,
                                                   find_transactions_req_t const *const req, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("findTransactions"));

  ret = hash243_queue_to_json_array(req->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(req->approvees, json_root, "approvees");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(req->bundles, json_root, "bundles");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash81_queue_to_json_array(req->tags, json_root, "tags");
  if (ret != RC_OK) {
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

retcode_t json_find_transactions_deserialize_request(serializer_t const *const s, char const *const obj,
                                                     find_transactions_req_t *req) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "bundles", &req->bundles);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "addresses", &req->addresses);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash81_queue(json_obj, "tags", &req->tags);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "approvees", &req->approvees);

end:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_find_transactions_serialize_response(serializer_t const *const s, find_transactions_res_t *const res,
                                                    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  char const *json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = hash243_queue_to_json_array(res->hashes, json_root, "hashes");
  if (ret != RC_OK) {
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

retcode_t json_find_transactions_deserialize_response(serializer_t const *const s, char const *const obj,
                                                      find_transactions_res_t *res) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "hashes", &res->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
