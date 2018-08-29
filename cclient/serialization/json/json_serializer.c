/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "json_serializer.h"

void init_json_serializer(serializer_t* serializer) {
  serializer->vtable = json_vtable;
}

retcode_t json_array_to_utarray(cJSON* obj, const char* obj_name,
                                UT_array* ut) {
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        utarray_push_back(ut, &current_obj->valuestring);
      }
    }
  } else {
    cJSON_Delete(obj);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}

retcode_t utarray_to_json_array(UT_array* ut, cJSON* json_root,
                                const char* obj_name) {
  if (utarray_len(ut) > 0) {
    cJSON* array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }

    cJSON_AddItemToObject(json_root, obj_name, array_obj);
    char** p = NULL;
    while ((p = (char**)utarray_next(ut, p))) {
      cJSON_AddItemToArray(array_obj, cJSON_CreateString(*p));
    }
  }
  return RC_OK;
}

retcode_t json_get_size(const cJSON* json_obj, const char* obj_name,
                        size_t* num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value)) {
    *num = (size_t)json_value->valuedouble;
  } else {
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_int(const cJSON* json_obj, const char* obj_name, int* num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value)) {
    *num = json_value->valueint;
  } else {
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_string(cJSON* json_obj, char* obj_name,
                          char_buffer_t* text) {
  retcode_t ret = RC_OK;
  size_t str_len = 0;
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) return RC_CCLIENT_JSON_KEY;

  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    str_len = strlen(json_value->valuestring);
    ret = char_buffer_allocate(text, str_len);
    if (ret != RC_OK) {
      return ret;
    }

    strcpy(text->data, json_value->valuestring);
  } else {
    return RC_CCLIENT_JSON_PARSE;
  }

  return ret;
}

retcode_t json_find_transactions_serialize_request(
    const serializer_t* const s, const find_transactions_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("findTransactions"));

  ret = utarray_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  ret = utarray_to_json_array(obj->approvees, json_root, "approvees");
  if (ret != RC_OK) {
    goto err;
  }

  ret = utarray_to_json_array(obj->bundles, json_root, "bundles");
  if (ret != RC_OK) {
    goto err;
  }

  ret = utarray_to_json_array(obj->tags, json_root, "tags");
  if (ret != RC_OK) {
    goto err;
  }

  json_text = cJSON_PrintUnformatted(json_root);
  if (json_text) {
    len = strlen(json_text);
    ret = char_buffer_allocate(out, len);
    if (ret == RC_OK) {
      strncpy(out->data, json_text, len);
    }
    cJSON_free((void*)json_text);
  }

  cJSON_Delete(json_root);
  return ret;

err:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_find_transactions_deserialize_response(
    const serializer_t* const s, const char* const obj,
    find_transactions_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    // TODO log the error message from response.
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  retcode_t ret = json_array_to_utarray(json_obj, "hashes", out->hashes);
  if (ret != RC_OK) {
    return ret;
  }

  return RC_OK;
}

// get_balances_response
void json_get_balances_serialize_request(const serializer_t* const s,
                                         const get_balances_req_t* const obj,
                                         char* out) {}

void json_get_balances_deserialize_response(const serializer_t* const s,
                                            const char* const obj,
                                            get_balances_res_t* out) {}

// get_inclusion_state_response
void json_get_inclusion_state_serialize_request(
    const serializer_t* const s, const get_inclusion_state_req_t* const obj,
    char* out) {}

void json_get_inclusion_state_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_inclusion_state_res_t* out) {}

retcode_t json_get_neighbors_serialize_request(const serializer_t* const s,
                                               char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* req_text = "{\"command\":\"getNeighbors\"}";
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

// get_neighbors_response

retcode_t json_get_neighbors_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  get_neighbors_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    // TODO log the error message from response.
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "neighbors");
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      char_buffer_t* addr = char_buffer_new();
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

retcode_t json_get_node_info_serialize_request(const serializer_t* const s,
                                               char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* req_text = "{\"command\":\"getNodeInfo\"}";
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_node_info_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  get_node_info_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    // TODO log the error message from response.
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

  ret = json_get_int(json_obj, "jreAvailableProcessors",
                     &out->jre_available_processors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreFreeMemory", &out->jre_free_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreMaxMemory", &out->jre_max_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreTotalMemory", &out->jre_total_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, "latestMilestone", out->latest_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "latestMilestoneIndex",
                      &out->latest_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, "latestSolidSubtangleMilestone",
                        out->latest_solid_subtangle_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "latestSolidSubtangleMilestoneIndex",
                      &out->latest_solid_subtangle_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "neighbors", &out->neighbors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "packetsQueueSize", &out->packets_queue_size);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "time", &out->time);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "tips", &out->tips);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "transactionsToRequest", &out->trans_to_request);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

// get_tips_response
void json_get_tips_deserialize_response(const serializer_t* const s,
                                        const char* const obj,
                                        get_tips_res_t* out) {}

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(
    const serializer_t* const s, int depth, char* out) {}

void json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_transactions_to_approve_res_t* out) {}

retcode_t json_add_neighbors_serialize_request(const serializer_t* const s,
                                               add_neighbors_req_t* obj,
                                               char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("addNeighbors"));

  ret = utarray_to_json_array(obj, json_root, "uris");
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
    cJSON_free((void*)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_add_neighbors_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  add_neighbors_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    // TODO log the error message from response.
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_int(json_obj, "addedNeighbors", out);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_remove_neighbors_serialize_request(const serializer_t* const s,
                                                  remove_neighbors_req_t* obj,
                                                  char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("removeNeighbors"));

  ret = utarray_to_json_array(obj, json_root, "uris");
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
    cJSON_free((void*)json_text);
  }

  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_remove_neighbors_deserialize_response(
    const serializer_t* const s, const char* const obj,
    remove_neighbors_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    // TODO log the error message from response.
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_int(json_obj, "removedNeighbors", out);

  cJSON_Delete(json_obj);
  return ret;
}
