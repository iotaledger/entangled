/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_node_info.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static const char *kAppName = "appName";
static const char *kAppVersion = "appVersion";
static const char *kLatestMilestone = "latestMilestone";
static const char *kLatestMilestoneIdx = "latestMilestoneIndex";
static const char *kLatestSolidSubtangleMilestone =
    "latestSolidSubtangleMilestone";
static const char *kLatestSolidSubtangleMilestoneIdx =
    "latestSolidSubtangleMilestoneIndex";
static const char *kMilestoneStartIdx = "milestoneStartIndex";
static const char *kNeighbors = "neighbors";
static const char *kPacketsQueueSize = "packetsQueueSize";
static const char *kTime = "time";
static const char *kTips = "tips";
static const char *kTransactionsToRequest = "transactionsToRequest";
static const char *kCoordinatorAddress = "coordinatorAddress";

retcode_t json_get_node_info_serialize_request(const serializer_t *const s,
                                               char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *req_text = "{\"command\":\"getNodeInfo\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_node_info_serialize_response(
    const serializer_t *const s, const get_node_info_res_t *const obj,
    char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddStringToObject(json_root, kAppName, obj->app_name->data);
  cJSON_AddStringToObject(json_root, kAppVersion, obj->app_version->data);

  ret = flex_trits_to_json_string(json_root, kLatestMilestone,
                                  obj->latest_milestone, HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
  }

  cJSON_AddNumberToObject(json_root, kLatestMilestoneIdx,
                          obj->latest_milestone_index);

  ret = flex_trits_to_json_string(json_root, kLatestSolidSubtangleMilestone,
                                  obj->latest_solid_subtangle_milestone,
                                  HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
  }
  cJSON_AddNumberToObject(json_root, kLatestSolidSubtangleMilestoneIdx,
                          obj->latest_solid_subtangle_milestone_index);

  cJSON_AddNumberToObject(json_root, kMilestoneStartIdx,
                          obj->milestone_start_index);
  cJSON_AddNumberToObject(json_root, kNeighbors, obj->neighbors);
  cJSON_AddNumberToObject(json_root, kPacketsQueueSize,
                          obj->packets_queue_size);
  cJSON_AddNumberToObject(json_root, kTime, obj->time);
  cJSON_AddNumberToObject(json_root, kTips, obj->tips);
  cJSON_AddNumberToObject(json_root, kTransactionsToRequest,
                          obj->transactions_to_request);

  ret = flex_trits_to_json_string(json_root, kCoordinatorAddress,
                                  obj->coordinator_address, HASH_LENGTH_TRIT);
  if (ret != RC_OK) {
    goto done;
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

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_node_info_deserialize_response(const serializer_t *const s,
                                                  const char *const obj,
                                                  get_node_info_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_string(json_obj, kAppName, out->app_name);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, kAppVersion, out->app_version);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, kLatestMilestone,
                                       out->latest_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, kLatestMilestoneIdx,
                        &out->latest_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, kLatestSolidSubtangleMilestone,
                                       out->latest_solid_subtangle_milestone);
  if (ret != RC_OK) {
    goto end;
  }
  ret = json_get_uint32(json_obj, kLatestSolidSubtangleMilestoneIdx,
                        &out->latest_solid_subtangle_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, kMilestoneStartIdx,
                        &out->milestone_start_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, kNeighbors, &out->neighbors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, kPacketsQueueSize, &out->packets_queue_size);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint64(json_obj, kTime, &out->time);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, kTips, &out->tips);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, kTransactionsToRequest,
                        &out->transactions_to_request);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, kCoordinatorAddress,
                                       out->coordinator_address);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
