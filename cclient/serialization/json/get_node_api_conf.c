/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_node_api_conf.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static char const *max_find_transactions = "maxFindTransactions";
static char const *max_requests_list = "maxRequestsList";
static char const *max_get_trytes = "maxGetTrytes";
static char const *max_body_length = "maxBodyLength";
static char const *milestone_start_index = "milestoneStartIndex";
static char const *test_net = "testNet";

retcode_t json_get_node_api_conf_serialize_request(char_buffer_t *out) {
  char const *req_text = "{\"command\":\"getNodeAPIConfiguration\"}";
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }
  return char_buffer_set(out, req_text);
}

retcode_t json_get_node_api_conf_serialize_response(get_node_api_conf_res_t const *const obj, char_buffer_t *out) {
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddNumberToObject(json_root, max_find_transactions, obj->max_find_transactions);
  cJSON_AddNumberToObject(json_root, max_requests_list, obj->max_requests_list);
  cJSON_AddNumberToObject(json_root, max_get_trytes, obj->max_get_trytes);
  cJSON_AddNumberToObject(json_root, max_body_length, obj->max_body_length);
  cJSON_AddNumberToObject(json_root, milestone_start_index, obj->milestone_start_index);
  cJSON_AddBoolToObject(json_root, test_net, obj->test_net);

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return RC_OK;
}

retcode_t json_get_node_api_conf_deserialize_response(char const *const obj, get_node_api_conf_res_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  if ((ret = json_get_uint32(json_obj, max_find_transactions, &out->max_find_transactions)) != RC_OK) {
    goto end;
  }

  if ((ret = json_get_uint32(json_obj, max_requests_list, &out->max_requests_list)) != RC_OK) {
    goto end;
  }

  if ((ret = json_get_uint32(json_obj, max_get_trytes, &out->max_get_trytes)) != RC_OK) {
    goto end;
  }

  if ((ret = json_get_uint32(json_obj, max_body_length, &out->max_body_length)) != RC_OK) {
    goto end;
  }

  if ((ret = json_get_uint32(json_obj, milestone_start_index, &out->milestone_start_index)) != RC_OK) {
    goto end;
  }

  if ((ret = json_get_boolean(json_obj, test_net, &out->test_net)) != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
