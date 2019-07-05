/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_transactions_to_approve.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_transactions_to_approve_serialize_request(get_transactions_to_approve_req_t const *const obj,
                                                             char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("getTransactionsToApprove"));

  cJSON_AddItemToObject(json_root, "depth", cJSON_CreateNumber(obj->depth));

  if (obj->reference) {
    ret = flex_trits_to_json_string(json_root, "reference", obj->reference, NUM_TRITS_HASH);
    if (ret != RC_OK) {
      goto done;
    }
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

retcode_t json_get_transactions_to_approve_deserialize_request(char const *const obj,
                                                               get_transactions_to_approve_req_t *out) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_get_uint32(json_obj, "depth", &out->depth);
  if (ret != RC_OK) {
    goto end;
  }

  if (cJSON_HasObjectItem(json_obj, "reference")) {
    flex_trit_t hash[FLEX_TRIT_SIZE_243];

    if ((ret = json_string_hash_to_flex_trits(json_obj, "reference", hash)) == RC_OK) {
      ret = get_transactions_to_approve_req_set_reference(out, hash);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_transactions_to_approve_serialize_response(get_transactions_to_approve_res_t const *const obj,
                                                              char_buffer_t *out) {
  retcode_t ret = RC_ERROR;
  char const *json_text = NULL;

  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  ret = flex_trits_to_json_string(json_root, "trunkTransaction", obj->trunk, NUM_TRITS_HASH);
  if (ret != RC_OK) {
    goto done;
  }
  ret = flex_trits_to_json_string(json_root, "branchTransaction", obj->branch, NUM_TRITS_HASH);
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

retcode_t json_get_transactions_to_approve_deserialize_response(char const *const obj,
                                                                get_transactions_to_approve_res_t *out) {
  retcode_t ret = RC_ERROR;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_string_hash_to_flex_trits(json_obj, "trunkTransaction", out->trunk);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_hash_to_flex_trits(json_obj, "branchTransaction", out->branch);

end:
  cJSON_Delete(json_obj);
  return ret;
}
