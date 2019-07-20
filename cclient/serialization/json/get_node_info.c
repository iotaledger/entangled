/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_node_info.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static char const *app_name = "appName";
static char const *app_version = "appVersion";
static char const *latest_milestone = "latestMilestone";
static char const *latest_milestone_idx = "latestMilestoneIndex";
static char const *latest_solid_subtangle_milestone = "latestSolidSubtangleMilestone";
static char const *latest_solid_subtangle_milestone_idx = "latestSolidSubtangleMilestoneIndex";
static char const *milestone_start_idx = "milestoneStartIndex";
static char const *neighbors = "neighbors";
static char const *packets_queue_size = "packetsQueueSize";
static char const *time = "time";
static char const *tips = "tips";
static char const *transactions_to_request = "transactionsToRequest";
static char const *features = "features";
static char const *coordinator_address = "coordinatorAddress";

retcode_t json_get_node_info_serialize_request(char_buffer_t *out) {
  char const *req_text = "{\"command\":\"getNodeInfo\"}";
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  if (!out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }
  return char_buffer_set(out, req_text);
}

retcode_t json_get_node_info_serialize_response(get_node_info_res_t const *const obj, char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
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

  cJSON_AddStringToObject(json_root, app_name, get_node_info_res_app_name(obj));
  cJSON_AddStringToObject(json_root, app_version, get_node_info_res_app_version(obj));

  ret = flex_trits_to_json_string(json_root, latest_milestone, get_node_info_res_lm(obj), HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
  }

  cJSON_AddNumberToObject(json_root, latest_milestone_idx, obj->latest_milestone_index);

  ret = flex_trits_to_json_string(json_root, latest_solid_subtangle_milestone, get_node_info_res_lssm(obj),
                                  HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
  }
  cJSON_AddNumberToObject(json_root, latest_solid_subtangle_milestone_idx, obj->latest_solid_subtangle_milestone_index);

  cJSON_AddNumberToObject(json_root, milestone_start_idx, obj->milestone_start_index);
  cJSON_AddNumberToObject(json_root, neighbors, obj->neighbors);
  cJSON_AddNumberToObject(json_root, packets_queue_size, obj->packets_queue_size);
  cJSON_AddNumberToObject(json_root, time, obj->time);
  cJSON_AddNumberToObject(json_root, tips, obj->tips);
  cJSON_AddNumberToObject(json_root, transactions_to_request, obj->transactions_to_request);

  ret = utarray_to_json_array(obj->features, json_root, features);
  if (ret != RC_OK) {
    goto done;
  }

  ret = flex_trits_to_json_string(json_root, coordinator_address, obj->coordinator_address, HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
  }

  char const *json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_node_info_deserialize_response(char const *const obj, get_node_info_res_t *out) {
  retcode_t ret = RC_ERROR;
  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (!obj || !out) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, error_2_string(RC_NULL_PARAM));
    return RC_NULL_PARAM;
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_string(json_obj, app_name, out->app_name);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, app_version, out->app_version);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, latest_milestone, out->latest_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, latest_milestone_idx, &out->latest_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret =
      json_string_hash_to_flex_trits(json_obj, latest_solid_subtangle_milestone, out->latest_solid_subtangle_milestone);
  if (ret != RC_OK) {
    goto end;
  }
  ret = json_get_uint32(json_obj, latest_solid_subtangle_milestone_idx, &out->latest_solid_subtangle_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, milestone_start_idx, &out->milestone_start_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, neighbors, &out->neighbors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, packets_queue_size, &out->packets_queue_size);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint64(json_obj, time, &out->time);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, tips, &out->tips);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, transactions_to_request, &out->transactions_to_request);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_array_to_utarray(json_obj, features, out->features);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, coordinator_address, out->coordinator_address);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
