#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "json_serializer.h"

int address_to_flex_trit(address_t addr, const char* trytes) {
  int num_trytes = strlen(trytes);
  int ret_trytes =
      tryte_to_flex_trit(addr->trits, num_trytes * 3,
                         (const signed char*)trytes, num_trytes, num_trytes);
  if (ret_trytes == num_trytes) {
    addr->num_trits = ret_trytes * 3;
    addr->num_bytes = trit_array_bytes_for_trits(ret_trytes * 3);
  } else {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void init_json_serializer(serializer_t* serializer) {
  serializer->vtable = json_vtable;
}

void json_find_transactions_serialize_request(
    const serializer_t* const s, const find_transactions_req_t* const obj,
    char* out) {}

int json_find_transactions_deserialize_response(const serializer_t* const s,
                                                const char* const obj,
                                                find_transactions_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "hashes");
  if (cJSON_IsArray(json_item)) {
    int hash_items = cJSON_GetArraySize(json_item);
    if (hash_items <= 0) {
      cJSON_Delete(json_obj);
      return EXIT_FAILURE;
    }

    cJSON* current_obj = NULL;
    int addr_count = 0;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->string != NULL) {
        address_to_flex_trit(out->transactions.array[addr_count],
                             current_obj->string);
        if (addr_count >= out->transactions.size) break;
        addr_count++;
      }
    }
  } else {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
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

int json_get_balances_deserialize_response(const serializer_t* const s,
                                           const char* const obj,
                                           get_balances_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "balances");

  if (cJSON_IsArray(json_item)) {
    int int_items = cJSON_GetArraySize(json_item);
    if (int_items <= 0) {
      cJSON_Delete(json_obj);
      return EXIT_FAILURE;
    }
  }

  int int_cnt = 0;
  cJSON* current_obj = NULL;
  cJSON_ArrayForEach(current_obj, json_item) {
    if (current_obj->valueint) {
      out->balances.array[int_cnt] = current_obj->valueint;
      if (int_cnt >= out->balances.size) break;
      int_cnt++;
    }
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "milestoneIndex");
  out->milestoneIndex = json_item->valueint;

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "mileston");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    address_to_flex_trit(out->milestone, json_item->string);
  }

  return EXIT_SUCCESS;
}

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

int json_get_inclusion_state_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_inclusion_state_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "states");

  if (cJSON_IsArray(json_item)) {
    int int_items = cJSON_GetArraySize(json_item);
    if (int_items <= 0) {
      cJSON_Delete(json_obj);
      return EXIT_FAILURE;
    }
  }

  int int_cnt = 0;
  cJSON* current_obj = NULL;
  cJSON_ArrayForEach(current_obj, json_item) {
    if (current_obj->string != NULL) {
      out->bitmap.array[int_cnt] = strcmp("true", current_obj->string) ? 1 : 0;
      if (int_cnt >= out->bitmap.size) break;
      int_cnt++;
    }
  }
  return EXIT_SUCCESS;
}

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

int json_get_node_info_deserialize_response(const serializer_t* const s,
                                            const char* const obj,
                                            get_node_info_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_value = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "appName");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    strcpy(out->appName, json_value->string);
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "appVersion");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    strcpy(out->appVersion, json_value->string);
  }

  json_value =
      cJSON_GetObjectItemCaseSensitive(json_obj, "jreAvailableProcessors");
  if (cJSON_IsNumber(json_value)) {
    out->jreAvailableProcessors = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "jreFreeMemory");
  if (cJSON_IsNumber(json_value)) {
    out->jreFreeMemory = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "jreMaxMemory");
  if (cJSON_IsNumber(json_value)) {
    out->jreMaxMemory = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "jreTotalMemory");
  if (cJSON_IsNumber(json_value)) {
    out->jreTotalMemory = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "latestMilestone");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    address_to_flex_trit(out->latestMilestone, json_value->string);
  }

  json_value =
      cJSON_GetObjectItemCaseSensitive(json_obj, "latestMilestoneIndex");
  if (cJSON_IsNumber(json_value)) {
    out->latestMilestoneIndex = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(
      json_obj, "latestSolidSubtangleMilestone");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    address_to_flex_trit(out->latestSolidSubtangleMilestone,
                         json_value->string);
  }

  json_value =
      cJSON_GetObjectItemCaseSensitive(json_obj, "latestMilestoneIndex");
  if (cJSON_IsNumber(json_value)) {
    out->latestMilestoneIndex = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "neighbors");
  if (cJSON_IsNumber(json_value)) {
    out->neighbors = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "packetsQueueSize");
  if (cJSON_IsNumber(json_value)) {
    out->packetsQueueSize = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "time");
  if (cJSON_IsNumber(json_value)) {
    out->time = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "tips");
  if (cJSON_IsNumber(json_value)) {
    out->tips = json_value->valueint;
  }

  json_value =
      cJSON_GetObjectItemCaseSensitive(json_obj, "transactionsToRequest");
  if (cJSON_IsNumber(json_value)) {
    out->transactionsToRequest = json_value->valueint;
  }

  return EXIT_SUCCESS;
}

size_t json_get_node_info_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_tips_response
int json_get_tips_deserialize_response(const serializer_t* const s,
                                       const char* const obj,
                                       get_tips_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "hashes");
  if (cJSON_IsArray(json_item)) {
    int hash_items = cJSON_GetArraySize(json_item);
    if (hash_items <= 0) {
      cJSON_Delete(json_obj);
      return EXIT_FAILURE;
    }

    cJSON* current_obj = NULL;
    int addr_cnt = 0;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->string != NULL) {
        address_to_flex_trit(out->tips.array[addr_cnt], current_obj->string);
        if (addr_cnt >= out->tips.size) break;
        addr_cnt++;
      }
    }
  } else {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

size_t json_get_tips_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(
    const serializer_t* const s, int depth, char* out) {}

int json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_transactions_to_approve_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_value = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    cJSON_Delete(json_obj);
    return EXIT_FAILURE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "trunkTransaction");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    address_to_flex_trit(out->branchTransaction, json_value->string);
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "branchTransaction");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    address_to_flex_trit(out->branchTransaction, json_value->string);
  }
  return EXIT_SUCCESS;
}

size_t json_get_transactions_to_approve_serialize_request_get_size(
    const serializer_t* const s) {
  return 0;
}

size_t json_get_transactions_to_approve_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}
