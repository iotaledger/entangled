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
    if (array_obj == NULL) return RC_CCLIENT_JSON_CREATE;

    cJSON_AddItemToObject(json_root, obj_name, array_obj);
    char** p = NULL;
    while ((p = (char**)utarray_next(ut, p))) {
      cJSON_AddItemToArray(array_obj, cJSON_CreateString(*p));
    }
  }
  return RC_OK;
}

retcode_t json_find_transactions_serialize_request(
    const serializer_t* const s, const find_transactions_req_t* const obj,
    char_buffer* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) return RC_CCLIENT_JSON_CREATE;

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("findTransactions"));

  ret = utarray_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) goto err;

  ret = utarray_to_json_array(obj->approvees, json_root, "approvees");
  if (ret != RC_OK) goto err;

  ret = utarray_to_json_array(obj->bundles, json_root, "bundles");
  if (ret != RC_OK) goto err;

  ret = utarray_to_json_array(obj->tags, json_root, "tags");
  if (ret != RC_OK) goto err;

  json_text = cJSON_PrintUnformatted(json_root);
  len = strlen(json_text);
  ret = char_buffer_allocate(out, len);
  if (ret == RC_OK) {
    strncpy(out->data, json_text, len);
  }

  if (json_text) cJSON_free((void*)json_text);
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
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  retcode_t ret = json_array_to_utarray(json_obj, "hashes", out->hashes);
  if (ret != RC_OK) return ret;

  return RC_OK;
}

size_t json_find_transactions_serialize_request_get_size(
    const serializer_t* const s,
    const find_transactions_req_t* const toSerialize) {
  return 0;
}

size_t json_find_transactions_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_balances_response
void json_get_balances_serialize_request(const serializer_t* const s,
                                         const get_balances_req_t* const obj,
                                         char* out) {}

void json_get_balances_deserialize_response(const serializer_t* const s,
                                            const char* const obj,
                                            get_balances_res_t* out) {}

size_t json_get_balances_serialize_request_get_size_(
    const serializer_t* const s, const get_balances_req_t* const toSerialize) {
  return 0;
}

size_t json_get_balances_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_inclusion_state_response
void json_get_inclusion_state_serialize_request(
    const serializer_t* const s, const get_inclusion_state_req_t* const obj,
    char* out) {}

void json_get_inclusion_state_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_inclusion_state_res_t* out) {}

size_t json_get_inclusion_state_serialize_request_get_size(
    const serializer_t* const s,
    const get_inclusion_state_req_t* const toSerialize) {
  return 0;
}

size_t json_get_inclusion_state_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_neighbors_response

void json_get_neighbors_deserialize_response(const serializer_t* const s,
                                             const char* const obj,
                                             get_neighbors_res_t* out) {}

size_t json_get_neighbors_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_node_info
size_t json_get_node_info_serialize_request_get_size(
    const serializer_t* const s) {
  return sizeof("{\"command\": \"getNodeInfo\"}");
}
void json_get_node_info_serialize_request(const serializer_t* const s,
                                          char* out) {
  strcpy(out, "{\"command\": \"getNodeInfo\"}");
}

void json_get_node_info_deserialize_response(const serializer_t* const s,
                                             const char* const obj,
                                             get_node_info_res_t* out) {}

size_t json_get_node_info_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_tips_response
void json_get_tips_deserialize_response(const serializer_t* const s,
                                        const char* const obj,
                                        get_tips_res_t* out) {}

size_t json_get_tips_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(
    const serializer_t* const s, int depth, char* out) {}

void json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_transactions_to_approve_res_t* out) {}

size_t json_get_transactions_to_approve_serialize_request_get_size(
    const serializer_t* const s) {
  return 0;
}

size_t json_get_transactions_to_approve_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}
