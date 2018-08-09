#include <stdio.h>
#include <string.h>

#include "cJSON.h"
#include "json_serializer.h"

void init_json_serializer(serializer_t* serializer) {
  serializer->vtable = json_vtable;
}

void json_find_transactions_serialize_request(
    const serializer_t* const s, const find_transactions_req_t* const obj,
    char* out) {}

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
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "hashes");
  if (cJSON_IsArray(json_item)) {
    int hash_items = cJSON_GetArraySize(json_item);
    if (hash_items <= 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }

    cJSON* current_obj = NULL;
    int addr_count = 1;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->string != NULL) {
        if ((addr_count > out->transactions.size) || (addr_count > hash_items))
          break;

        int ret_trytes = tryte_to_flex_trit(
            out->transactions.trits + (addr_count * sizeof(flex_trit_p)),
            NUM_TRITS_ADDRESS, (const signed char*)current_obj->string,
            strlen(current_obj->string), NUM_TRYTES_ADDRESS);
        if (ret_trytes == 0) {
          cJSON_Delete(json_obj);
          return RC_CCLIENT_JSON_PARSE;
        }

        addr_count++;
      }
    }
  } else {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

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

retcode_t json_get_balances_deserialize_response(const serializer_t* const s,
                                                 const char* const obj,
                                                 get_balances_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "balances");

  int int_items = 0;
  if (cJSON_IsArray(json_item)) {
    int_items = cJSON_GetArraySize(json_item);
    if (int_items <= 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }

  int int_cnt = 0;
  cJSON* current_obj = NULL;
  cJSON_ArrayForEach(current_obj, json_item) {
    if (current_obj->valueint) {
      if ((int_cnt >= out->balances.size) || (int_cnt >= int_items)) break;
      out->balances.integers[int_cnt] = current_obj->valueint;
      int_cnt++;
    }
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "milestoneIndex");
  out->milestoneIndex = json_item->valueint;

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "mileston");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    int ret_trytes =
        tryte_to_flex_trit(out->milestone, NUM_TRITS_ADDRESS,
                           (const signed char*)json_item->string,
                           strlen(json_item->string), NUM_TRYTES_ADDRESS);
    if (ret_trytes == 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }

  return RC_OK;
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

retcode_t json_get_inclusion_state_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_inclusion_state_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "states");

  int int_items = 0;
  if (cJSON_IsArray(json_item)) {
    int_items = cJSON_GetArraySize(json_item);
    if (int_items <= 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }

  int int_cnt = 0;
  cJSON* current_obj = NULL;
  cJSON_ArrayForEach(current_obj, json_item) {
    if ((int_cnt >= out->states.size) || (int_cnt >= int_items)) break;
    if (current_obj->string != NULL) {
      out->states.integers[int_cnt] =
          strcmp("true", current_obj->string) ? 1 : 0;
    }
    int_cnt++;
  }
  return RC_OK;
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

retcode_t json_get_node_info_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  get_node_info_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_value = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
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
    int ret_trytes =
        tryte_to_flex_trit(out->latestMilestone, NUM_TRITS_ADDRESS,
                           (const signed char*)json_value->string,
                           strlen(json_value->string), NUM_TRYTES_ADDRESS);
    if (ret_trytes == 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }

  json_value =
      cJSON_GetObjectItemCaseSensitive(json_obj, "latestMilestoneIndex");
  if (cJSON_IsNumber(json_value)) {
    out->latestMilestoneIndex = json_value->valueint;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(
      json_obj, "latestSolidSubtangleMilestone");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    int ret_trytes = tryte_to_flex_trit(
        out->latestSolidSubtangleMilestone, NUM_TRITS_ADDRESS,
        (const signed char*)json_value->string, strlen(json_value->string),
        NUM_TRYTES_ADDRESS);
    if (ret_trytes == 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
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

  return RC_OK;
}

size_t json_get_node_info_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_tips_response
retcode_t json_get_tips_deserialize_response(const serializer_t* const s,
                                             const char* const obj,
                                             get_tips_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "hashes");
  if (cJSON_IsArray(json_item)) {
    int hash_items = cJSON_GetArraySize(json_item);
    if (hash_items <= 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }

    cJSON* current_obj = NULL;
    int addr_cnt = 1;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->string != NULL) {
        if ((addr_cnt > out->tips.size) || (addr_cnt > hash_items)) break;
        int ret_trytes = tryte_to_flex_trit(
            out->tips.trits + (addr_cnt * sizeof(flex_trit_p)),
            NUM_TRITS_ADDRESS, (const signed char*)current_obj->string,
            strlen(current_obj->string), NUM_TRYTES_ADDRESS);
        if (ret_trytes == 0) {
          cJSON_Delete(json_obj);
          return RC_CCLIENT_JSON_PARSE;
        }

        addr_cnt++;
      }
    }
  } else {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

size_t json_get_tips_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}

// get_transactions_to_approve_response
void json_get_transactions_to_approve_serialize_request(
    const serializer_t* const s, int depth, char* out) {}

retcode_t json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_transactions_to_approve_res_t* out) {
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_value = NULL;

  if (json_obj == NULL) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "trunkTransaction");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    int ret_trytes =
        tryte_to_flex_trit(out->trunkTransaction, NUM_TRITS_ADDRESS,
                           (const signed char*)json_value->string,
                           strlen(json_value->string), NUM_TRYTES_ADDRESS);
    if (ret_trytes == 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }

  json_value = cJSON_GetObjectItemCaseSensitive(json_obj, "branchTransaction");
  if (cJSON_IsString(json_value) && (json_value->string != NULL)) {
    int ret_trytes =
        tryte_to_flex_trit(out->branchTransaction, NUM_TRITS_ADDRESS,
                           (const signed char*)json_value->string,
                           strlen(json_value->string), NUM_TRYTES_ADDRESS);
    if (ret_trytes == 0) {
      cJSON_Delete(json_obj);
      return RC_CCLIENT_JSON_PARSE;
    }
  }
  return RC_OK;
}

size_t json_get_transactions_to_approve_serialize_request_get_size(
    const serializer_t* const s) {
  return 0;
}

size_t json_get_transactions_to_approve_deserialize_response_get_size(
    const serializer_t* const s, const char* const toDeserialize) {
  return 0;
}
