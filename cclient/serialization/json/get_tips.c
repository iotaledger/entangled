/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_tips.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_tips_serialize_request(const serializer_t *const s, char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *req_text = "{\"command\":\"getTips\"}";
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_tips_deserialize_response(const serializer_t *const s, const char *const obj, get_tips_res_t *res) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  ret = json_array_to_hash243_stack(json_obj, "hashes", &res->hashes);

  cJSON_Delete(json_obj);
  return ret;
}
