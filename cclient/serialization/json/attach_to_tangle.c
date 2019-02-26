/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/attach_to_tangle.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_attach_to_tangle_serialize_request(
    const serializer_t *const s, const attach_to_tangle_req_t *const obj,
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
                        cJSON_CreateString("attachToTangle"));

  ret = flex_trits_to_json_string(json_root, "trunkTransaction", obj->trunk,
                                  NUM_TRITS_HASH);
  if (ret != RC_OK) {
    goto done;
  }

  ret = flex_trits_to_json_string(json_root, "branchTransaction", obj->branch,
                                  NUM_TRITS_HASH);
  if (ret != RC_OK) {
    goto done;
  }

  cJSON_AddItemToObject(json_root, "minWeightMagnitude",
                        cJSON_CreateNumber(obj->mwm));

  ret = hash8019_array_to_json_array(obj->trytes, json_root, "trytes");
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

retcode_t json_attach_to_tangle_deserialize_response(
    const serializer_t *const s, const char *const obj,
    attach_to_tangle_res_t *const out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;
  log_info(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(json_logger_id, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }
  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "exception");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(json_logger_id, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_array_to_hash8019_array(json_obj, "trytes", out->trytes);

  cJSON_Delete(json_obj);
  return ret;
}
