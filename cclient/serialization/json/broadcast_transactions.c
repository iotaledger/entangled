/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/broadcast_transactions.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

static const char *kCmdName = "broadcastTransactions";
static const char *kTrytes = "trytes";

retcode_t json_broadcast_transactions_serialize_request(const serializer_t *const s,
                                                        broadcast_transactions_req_t *const req, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *json_text = NULL;
  size_t len = 0;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString(kCmdName));

  ret = hash8019_array_to_json_array(req->trytes, json_root, kTrytes);
  if (ret != RC_OK) {
    cJSON_Delete(json_root);
    return ret;
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
}

retcode_t json_broadcast_transactions_deserialize_request(const serializer_t *const s, const char *const obj,
                                                          broadcast_transactions_req_t *const out) {
  retcode_t ret = RC_OK;

  if (out->trytes == NULL) {
    out->trytes = hash8019_array_new();
  }

  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash8019_array(json_obj, kTrytes, out->trytes);

  cJSON_Delete(json_obj);
  return ret;
}
