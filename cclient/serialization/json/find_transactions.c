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
                                                   find_transactions_req_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("findTransactions"));

  ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(obj->approvees, json_root, "approvees");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(obj->bundles, json_root, "bundles");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash81_queue_to_json_array(obj->tags, json_root, "tags");
  if (ret != RC_OK) {
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

  cJSON_Delete(json_root);
  return ret;

err:
  log_error(json_logger_id, "[%s:%d] error!\n", __func__, __LINE__);
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_find_transactions_deserialize_response(serializer_t const *const s, char const *const obj,
                                                      find_transactions_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "hashes", &out->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
