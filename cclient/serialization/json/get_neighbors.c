/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */
#include "cclient/serialization/json/get_neighbors.h"

#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/json/logger.h"

retcode_t json_get_neighbors_serialize_request(const serializer_t *const s,
                                               char_buffer_t *out) {
  retcode_t ret = RC_OK;
  const char *req_text = "{\"command\":\"getNeighbors\"}";
  log_debug(json_logger_id, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_neighbors_deserialize_response(const serializer_t *const s,
                                                  const char *const obj,
                                                  get_neighbors_res_t *out) {
  retcode_t ret = RC_OK;
  cJSON *json_obj = cJSON_Parse(obj);
  cJSON *json_item = NULL;

  log_debug(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, obj);
  JSON_CHECK_ERROR(json_obj, json_item, json_logger_id);

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "neighbors");
  if (cJSON_IsArray(json_item)) {
    cJSON *current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      char_buffer_t *addr = char_buffer_new();
      int all_trans, invalid_trans, new_trans;
      ret = json_get_string(current_obj, "address", addr);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfAllTransactions", &all_trans);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfInvalidTransactions",
                         &invalid_trans);
      if (ret != RC_OK) {
        goto end;
      }
      ret = json_get_int(current_obj, "numberOfNewTransactions", &new_trans);
      if (ret != RC_OK) {
        goto end;
      }

      ret = get_neighbors_res_add_neighbor(out, addr, all_trans, invalid_trans,
                                           new_trans);
    }
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}
