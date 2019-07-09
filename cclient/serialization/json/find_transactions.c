/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/find_transactions.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_find_transactions_serialize_request(find_transactions_req_t const* const obj, char_buffer_t* out) {
  retcode_t ret = RC_ERROR;
  char const* json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  if (!obj->addresses && !obj->approvees && !obj->bundles && !obj->tags) {
    log_error(json_logger_id, "[%s:%d] Null parameters\n", __func__, __LINE__);
    ret = RC_NULL_PARAM;
    goto end;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("findTransactions"));

  if (obj->addresses) {
    ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
    if (ret != RC_OK) {
      goto end;
    }
  }

  if (obj->approvees) {
    ret = hash243_queue_to_json_array(obj->approvees, json_root, "approvees");
    if (ret != RC_OK) {
      goto end;
    }
  }

  if (obj->bundles) {
    ret = hash243_queue_to_json_array(obj->bundles, json_root, "bundles");
    if (ret != RC_OK) {
      goto end;
    }
  }

  if (obj->tags) {
    ret = hash81_queue_to_json_array(obj->tags, json_root, "tags");
    if (ret != RC_OK) {
      goto end;
    }
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void*)json_text);
  }

end:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_find_transactions_deserialize_request(char const* const obj, find_transactions_req_t* out) {
  retcode_t ret = RC_ERROR;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  if (!(cJSON_HasObjectItem(json_obj, "bundles") || cJSON_HasObjectItem(json_obj, "addresses") ||
        cJSON_HasObjectItem(json_obj, "tags") || cJSON_HasObjectItem(json_obj, "approvees"))) {
    ret = RC_NULL_PARAM;
    log_error(json_logger_id, "[%s:%d] Null parameters\n", __func__, __LINE__);
    goto end;
  }

  if (cJSON_HasObjectItem(json_obj, "bundles")) {
    ret = json_array_to_hash243_queue(json_obj, "bundles", &out->bundles);
    if (ret) {
      goto end;
    }
  }

  if (cJSON_HasObjectItem(json_obj, "addresses")) {
    ret = json_array_to_hash243_queue(json_obj, "addresses", &out->addresses);
    if (ret) {
      goto end;
    }
  }

  if (cJSON_HasObjectItem(json_obj, "tags")) {
    ret = json_array_to_hash81_queue(json_obj, "tags", &out->tags);
    if (ret) {
      goto end;
    }
  }

  if (cJSON_HasObjectItem(json_obj, "approvees")) {
    ret = json_array_to_hash243_queue(json_obj, "approvees", &out->approvees);
    if (ret) {
      goto end;
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_find_transactions_serialize_response(find_transactions_res_t const* const obj, char_buffer_t* out) {
  retcode_t ret = RC_ERROR;
  char const* json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();

  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    ret = RC_CCLIENT_JSON_CREATE;
    goto end;
  }

  ret = hash243_queue_to_json_array(obj->hashes, json_root, "hashes");
  if (ret != RC_OK) {
    goto end;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void*)json_text);
  }

end:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_find_transactions_deserialize_response(char const* const obj, find_transactions_res_t* out) {
  retcode_t ret = RC_ERROR;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "hashes", &out->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
