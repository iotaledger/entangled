/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/were_addresses_spent_from.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_were_addresses_spent_from_serialize_request(were_addresses_spent_from_req_t const *const req,
                                                           char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!req->addresses) {
    log_error(json_logger_id, "[%s:%d] Null parameter\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("wereAddressesSpentFrom"));

  ret = hash243_queue_to_json_array(req->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto done;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_were_addresses_spent_from_deserialize_request(char const *const obj,
                                                             were_addresses_spent_from_req_t *const req) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "addresses", &req->addresses);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_were_addresses_spent_from_serialize_response(were_addresses_spent_from_res_t const *const res,
                                                            char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = utarray_to_json_boolean_array(res->states, json_root, "states");
  if (ret != RC_OK) {
    goto done;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_were_addresses_spent_from_deserialize_response(char const *const obj,
                                                              were_addresses_spent_from_res_t *const res) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  if (!res->states) {
    utarray_new(res->states, &ut_int_icd);
  }
  if (!res->states) {
    return RC_OOM;
  }

  ret = json_boolean_array_to_utarray(json_obj, "states", res->states);

  cJSON_Delete(json_obj);
  return ret;
}
