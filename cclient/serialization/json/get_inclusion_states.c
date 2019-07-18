/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_inclusion_states.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_inclusion_states_serialize_request(get_inclusion_states_req_t const *const req, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!req || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  printf("tips = %p, tx = %p \n", req->tips, req->transactions);
  if (!req->transactions) {
    log_error(json_logger_id, "[%s:%d] NULL parameters\n", __func__, __LINE__);
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("getInclusionStates"));

  ret = hash243_queue_to_json_array(req->transactions, json_root, "transactions");
  if (ret != RC_OK) {
    goto done;
  }

  if (req->tips) {
    if ((ret = hash243_queue_to_json_array(req->tips, json_root, "tips")) != RC_OK) {
      goto done;
    }
  } else {
    cJSON *array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, "tips", array_obj);
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_inclusion_states_deserialize_request(char const *const obj, get_inclusion_states_req_t *const req) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!req || !obj) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_queue(json_obj, "transactions", &req->transactions);
  if (ret != RC_OK) {
    goto done;
  }

  if (cJSON_HasObjectItem(json_obj, "tips")) {
    ret = json_array_to_hash243_queue(json_obj, "tips", &req->tips);
  }

done:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_inclusion_states_serialize_response(get_inclusion_states_res_t const *const res,
                                                       char_buffer_t *out) {
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

  ret = utarray_to_json_boolean_array(res->states, json_root, "states");
  if (ret != RC_OK) {
    goto done;
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    ret = char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_inclusion_states_deserialize_response(char const *const obj, get_inclusion_states_res_t *const res) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!res || !obj) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
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
