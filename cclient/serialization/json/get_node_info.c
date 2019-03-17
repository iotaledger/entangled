/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_node_info.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_node_info_serialize_request(const serializer_t *const s, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *req_text = "{\"command\":\"getNodeInfo\"}";
  log_info(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_node_info_deserialize_response(const serializer_t *const s, const char *const obj,
                                                  get_node_info_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(json_logger_id, "[%s:%d] %s %s\n", __func__, __LINE__, STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_string(json_obj, "appName", out->app_name);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, "appVersion", out->app_version);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, "latestMilestone", out->latest_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, "latestMilestoneIndex", &out->latest_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret =
      json_string_hash_to_flex_trits(json_obj, "latestSolidSubtangleMilestone", out->latest_solid_subtangle_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, "latestSolidSubtangleMilestoneIndex", &out->latest_solid_subtangle_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, "milestoneStartIndex", &out->milestone_start_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, "neighbors", &out->neighbors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint16(json_obj, "packetsQueueSize", &out->packets_queue_size);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint64(json_obj, "time", &out->time);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, "tips", &out->tips);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_uint32(json_obj, "transactionsToRequest", &out->transactions_to_request);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, "coordinatorAddress", out->coordinator_address);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
