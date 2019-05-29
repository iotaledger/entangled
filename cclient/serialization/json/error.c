/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/error.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_error_serialize_response(error_res_t *const obj, char_buffer_t *out) {
  char const *json_text = NULL;
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);

  cJSON *json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddStringToObject(json_root, "error", obj->error->data);

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    char_buffer_set(out, json_text);
    cJSON_free((void *)json_text);
  }

  cJSON_Delete(json_root);
  return RC_OK;
}
