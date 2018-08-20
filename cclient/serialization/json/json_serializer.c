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

retcode_t json_find_transactions_serialize_request(
    const serializer_t* const s, const find_transactions_req_t* const obj,
    char_buffer* out) {
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) return RC_CCLIENT_JSON_CREATE;

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("findTransactions"));

  if (utarray_len(obj->addresses) > 0) {
    cJSON* addrs = cJSON_CreateArray();
    if (addrs == NULL) goto err;

    cJSON_AddItemToObject(json_root, "addresses", addrs);
    char** p = NULL;
    while ((p = (char**)utarray_next(obj->addresses, p))) {
      cJSON_AddItemToArray(addrs, cJSON_CreateString(*p));
    }
  }

  if (utarray_len(obj->approvees) > 0) {
    cJSON* approvees = cJSON_CreateArray();
    if (approvees == NULL) goto err;

    cJSON_AddItemToObject(json_root, "approvees", approvees);
    char** p = NULL;
    while ((p = (char**)utarray_next(obj->approvees, p))) {
      cJSON_AddItemToArray(approvees, cJSON_CreateString(*p));
    }
  }

  if (utarray_len(obj->bundles) > 0) {
    cJSON* bundles = cJSON_CreateArray();
    if (bundles == NULL) goto err;

    cJSON_AddItemToObject(json_root, "bundles", bundles);
    char** p = NULL;
    while ((p = (char**)utarray_next(obj->bundles, p))) {
      cJSON_AddItemToArray(bundles, cJSON_CreateString(*p));
    }
  }

  if (utarray_len(obj->tags) > 0) {
    cJSON* tags = cJSON_CreateArray();
    if (tags == NULL) goto err;

    cJSON_AddItemToObject(json_root, "tags", tags);
    char** p = NULL;
    while ((p = (char**)utarray_next(obj->tags, p))) {
      cJSON_AddItemToArray(tags, cJSON_CreateString(*p));
    }
  }

  const char* json_text = cJSON_PrintUnformatted(json_root);
  size_t len = strlen(json_text);
  retcode_t ret = char_buffer_allocate(out, len);
  if (ret == RC_OK) {
    strncpy(out->data, json_text, len);
  }

  if (json_text) cJSON_free((void*)json_text);
  cJSON_Delete(json_root);
  return ret;

err:
  cJSON_Delete(json_root);
  return RC_CCLIENT_JSON_CREATE;
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
