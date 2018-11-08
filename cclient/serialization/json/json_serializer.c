/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>
#include <string.h>

#include "cJSON.h"

#include "cclient/serialization/json/json_serializer.h"

#define JSON_LOGGER_ID "json_serializer"

void logger_init_json_serializer() {
  logger_helper_init(JSON_LOGGER_ID, LOGGER_DEBUG, true);
  log_info(JSON_LOGGER_ID, "[%s:%d] enable logger %s.\n", __func__, __LINE__,
           JSON_LOGGER_ID);
}

void logger_destroy_json_serializer() {
  log_info(JSON_LOGGER_ID, "[%s:%d] destroy logger %s.\n", __func__, __LINE__,
           JSON_LOGGER_ID);
  logger_helper_destroy(JSON_LOGGER_ID);
}

void init_json_serializer(serializer_t* serializer) {
  serializer->vtable = json_vtable;
}

static retcode_t json_array_to_utarray(cJSON const* const obj,
                                       char const* const obj_name,
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
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not array\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}

static retcode_t utarray_to_json_array(UT_array const* const ut,
                                       cJSON* const json_root,
                                       char const* const obj_name) {
  if (utarray_len(ut) > 0) {
    cJSON* array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                   STR_CCLIENT_JSON_CREATE);
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

static flex_hash_array_t* json_array_to_flex_hash_array(
    cJSON const* const obj, char const* const obj_name,
    flex_hash_array_t* head) {
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        head =
            flex_hash_array_append(head, (const char*)current_obj->valuestring);
      }
    }
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not array\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return NULL;
  }
  return head;
}

static retcode_t flex_hash_array_to_json_array(flex_hash_array_t* const head,
                                               cJSON* const json_root,
                                               char const* const obj_name) {
  flex_hash_array_t* elt;
  int array_count;
  cJSON* array_obj = NULL;

  LL_COUNT(head, elt, array_count);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);
    LL_FOREACH(head, elt) {
      // flex to trytes;
      size_t len_trytes = elt->hash->num_trits / 3;
      trit_t trytes_out[len_trytes + 1];
      size_t trits_count =
          flex_trits_to_trytes(trytes_out, len_trytes, elt->hash->trits,
                               elt->hash->num_trits, elt->hash->num_trits);
      trytes_out[len_trytes] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj,
                             cJSON_CreateString((const char*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

static retcode_t flex_hash_to_json_string(cJSON* const json_obj,
                                          char const* const key,
                                          trit_array_p const hash) {
  // flex to trytes;
  size_t len_trytes = hash->num_trits / 3;
  trit_t trytes_out[len_trytes + 1];
  size_t trits_count = flex_trits_to_trytes(trytes_out, len_trytes, hash->trits,
                                            hash->num_trits, hash->num_trits);
  trytes_out[len_trytes] = '\0';

  if (trits_count != 0) {
    cJSON_AddItemToObject(json_obj, key,
                          cJSON_CreateString((const char*)trytes_out));
  } else {
    return RC_CCLIENT_FLEX_TRITS;
  }
  return RC_OK;
}

static retcode_t json_string_to_flex_hash(cJSON const* const json_obj,
                                          char const* const key,
                                          trit_array_p const hash) {
  retcode_t ret = RC_OK;
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, key);
  if (json_value == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s.\n", __func__, __LINE__,
              STR_CCLIENT_JSON_KEY, key);
    return RC_CCLIENT_JSON_KEY;
  }
  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    ret = trytes_to_flex_hash(hash, json_value->valuestring);
    if (ret != RC_OK) {
      return ret;
    }
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not string\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret;
}

static retcode_t json_boolean_array_to_utarray(cJSON const* const obj,
                                               char const* const obj_name,
                                               UT_array* const ut) {
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    /* The type of json object is expected to be boolean type */
    cJSON_ArrayForEach(current_obj, json_item) {
      int bl = (int)cJSON_IsTrue(current_obj);
      utarray_push_back(ut, &bl);
    }
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not array\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}

static retcode_t json_get_size(cJSON const* const json_obj,
                               char const* const obj_name, size_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s.\n", __func__, __LINE__,
              STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value)) {
    *num = (size_t)json_value->valuedouble;
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not number\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

static retcode_t json_get_int(cJSON const* const json_obj,
                              char const* const obj_name, int* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s.\n", __func__, __LINE__,
              STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value)) {
    *num = json_value->valueint;
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not number\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

static retcode_t json_get_string(cJSON const* const json_obj,
                                 char const* const obj_name,
                                 char_buffer_t* const text) {
  retcode_t ret = RC_OK;
  size_t str_len = 0;
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s.\n", __func__, __LINE__,
              STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    str_len = strlen(json_value->valuestring);
    ret = char_buffer_allocate(text, str_len);
    if (ret != RC_OK) {
      log_error(JSON_LOGGER_ID, "[%s:%d] memory allocation failed.\n", __func__,
                __LINE__);
      return ret;
    }

    strcpy(text->data, json_value->valuestring);
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not string\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return ret;
}

static retcode_t hash243_queue_to_json_array(hash243_queue_t queue,
                                             cJSON* const json_root,
                                             char const* const obj_name) {
  size_t array_count;
  cJSON* array_obj = NULL;
  hash243_queue_entry_t* q_iter = NULL;

  array_count = hash243_queue_count(&queue);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    CDL_FOREACH(queue, q_iter) {
      trit_t trytes_out[NUM_TRYTES_HASH + 1];
      size_t trits_count =
          flex_trits_to_trytes(trytes_out, NUM_TRYTES_HASH, q_iter->hash,
                               NUM_TRITS_HASH, NUM_TRITS_HASH);
      trytes_out[NUM_TRYTES_HASH] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj,
                             cJSON_CreateString((const char*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

static retcode_t json_array_to_hash243_queue(cJSON const* const obj,
                                             char const* const obj_name,
                                             hash243_queue_t* queue) {
  retcode_t ret_code = RC_OK;
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        trit_array_p tmp_ref =
            trit_array_new_from_trytes((tryte_t*)current_obj->valuestring);
        ret_code = hash243_queue_push(queue, tmp_ref->trits);
        trit_array_free(tmp_ref);
        if (ret_code) {
          return ret_code;
        }
      }
    }
  } else {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s not array\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret_code;
}

static retcode_t hash81_queue_to_json_array(hash81_queue_t queue,
                                            cJSON* const json_root,
                                            char const* const obj_name) {
  size_t array_count;
  cJSON* array_obj = NULL;
  hash81_queue_entry_t* q_iter = NULL;

  array_count = hash81_queue_count(&queue);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    CDL_FOREACH(queue, q_iter) {
      trit_t trytes_out[NUM_TRYTES_TAG + 1];
      size_t trits_count =
          flex_trits_to_trytes(trytes_out, NUM_TRYTES_TAG, q_iter->hash,
                               NUM_TRITS_TAG, NUM_TRITS_TAG);
      trytes_out[NUM_TRYTES_TAG] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj,
                             cJSON_CreateString((const char*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t json_find_transactions_serialize_request(
    serializer_t const* const s, find_transactions_req_t const* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("findTransactions"));

  ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(obj->approvees, json_root, "approvees");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash243_queue_to_json_array(obj->bundles, json_root, "bundles");
  if (ret != RC_OK) {
    goto err;
  }

  ret = hash81_queue_to_json_array(obj->tags, json_root, "tags");
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
  log_error(JSON_LOGGER_ID, "[%s:%d] error!\n", __func__, __LINE__);
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_find_transactions_deserialize_response(
    serializer_t const* const s, char const* const obj,
    find_transactions_res_t* out) {
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_array_to_hash243_queue(json_obj, "hashes", &out->hashes);

  cJSON_Delete(json_obj);
  return ret;
}

// get_balances_response
retcode_t json_get_balances_serialize_request(
    serializer_t const* const s, get_balances_req_t const* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("getBalances"));

  ret = hash243_queue_to_json_array(obj->addresses, json_root, "addresses");
  if (ret != RC_OK) {
    goto err;
  }

  cJSON_AddItemToObject(json_root, "threshold",
                        cJSON_CreateNumber(obj->threshold));

  ret = hash243_queue_to_json_array(obj->tips, json_root, "tips");
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

retcode_t json_get_balances_deserialize_response(serializer_t const* const s,
                                                 char const* const obj,
                                                 get_balances_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_array_to_utarray(json_obj, "balances", out->balances);
  if (ret) {
    goto end;
  }

  ret = json_array_to_hash243_queue(json_obj, "references", &out->milestone);
  if (ret) {
    goto end;
  }

  ret = json_get_int(json_obj, "milestoneIndex", &out->milestoneIndex);
  if (ret != RC_OK) {
    goto end;
  }

end:

  cJSON_Delete(json_obj);
  return ret;
}

// get_inclusion_state_response
retcode_t json_get_inclusion_state_serialize_request(
    const serializer_t* const s, get_inclusion_state_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("getInclusionStates"));

  ret = flex_hash_array_to_json_array(obj->hashes, json_root, "transactions");
  if (ret != RC_OK) {
    goto done;
  }

  ret = flex_hash_array_to_json_array(obj->tips, json_root, "tips");
  if (ret != RC_OK) {
    goto done;
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

done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_get_inclusion_state_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_inclusion_state_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_boolean_array_to_utarray(json_obj, "states", out->states);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_neighbors_serialize_request(const serializer_t* const s,
                                               char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* req_text = "{\"command\":\"getNeighbors\"}";
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
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

  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);
  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
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
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_node_info_deserialize_response(const serializer_t* const s,
                                                  const char* const obj,
                                                  get_node_info_res_t** out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_string(json_obj, "appName", (*out)->app_name);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_string(json_obj, "appVersion", (*out)->app_version);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "jreAvailableProcessors",
                     &(*out)->jre_available_processors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreFreeMemory", &(*out)->jre_free_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreMaxMemory", &(*out)->jre_max_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "jreTotalMemory", &(*out)->jre_total_memory);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_to_flex_hash(json_obj, "latestMilestone",
                                 (*out)->latest_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "latestMilestoneIndex",
                      &(*out)->latest_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_to_flex_hash(json_obj, "latestSolidSubtangleMilestone",
                                 (*out)->latest_solid_subtangle_milestone);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "latestSolidSubtangleMilestoneIndex",
                      &(*out)->latest_solid_subtangle_milestone_index);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "neighbors", &(*out)->neighbors);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "packetsQueueSize", &(*out)->packets_queue_size);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_size(json_obj, "time", &(*out)->time);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "tips", &(*out)->tips);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_get_int(json_obj, "transactionsToRequest",
                     &(*out)->trans_to_request);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

// get_tips_response
retcode_t json_get_tips_serialize_request(const serializer_t* const s,
                                          char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* req_text = "{\"command\":\"getTips\"}";
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  ret = char_buffer_allocate(out, strlen(req_text));
  if (ret == RC_OK) {
    strcpy(out->data, req_text);
  }
  return ret;
}

retcode_t json_get_tips_deserialize_response(const serializer_t* const s,
                                             const char* const obj,
                                             get_tips_res_t* res) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  res->tips = json_array_to_flex_hash_array(json_obj, "hashes", res->tips);

  cJSON_Delete(json_obj);
  return ret;
}

// get_transactions_to_approve_response
retcode_t json_get_transactions_to_approve_serialize_request(
    const serializer_t* const s,
    const get_transactions_to_approve_req_t* const obj, char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("getTransactionsToApprove"));

  cJSON_AddItemToObject(json_root, "depth", cJSON_CreateNumber(obj->depth));

  cJSON_AddItemToObject(json_root, "reference",
                        cJSON_CreateString(obj->reference->data));

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

retcode_t json_get_transactions_to_approve_deserialize_response(
    const serializer_t* const s, const char* const obj,
    get_transactions_to_approve_res_t** out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_string_to_flex_hash(json_obj, "trunkTransaction", (*out)->trunk);
  if (ret != RC_OK) {
    goto end;
  }

  ret = json_string_to_flex_hash(json_obj, "branchTransaction", (*out)->branch);
  if (ret != RC_OK) {
    goto end;
  }

end:
  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_add_neighbors_serialize_request(
    const serializer_t* const s, const add_neighbors_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
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
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_int(json_obj, "addedNeighbors", out);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_remove_neighbors_serialize_request(
    const serializer_t* const s, const remove_neighbors_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
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
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  ret = json_get_int(json_obj, "removedNeighbors", out);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_get_trytes_serialize_request(const serializer_t* const s,
                                            get_trytes_req_t const* const req,
                                            char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command", cJSON_CreateString("getTrytes"));

  ret = flex_hash_array_to_json_array(req->hashes, json_root, "hashes");
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

retcode_t json_get_trytes_deserialize_response(const serializer_t* const s,
                                               const char* const obj,
                                               get_trytes_res_t* const res) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  res->trytes = json_array_to_flex_hash_array(json_obj, "trytes", res->trytes);

  cJSON_Delete(json_obj);
  return ret;
}

retcode_t json_attach_to_tangle_serialize_request(
    const serializer_t* const s, const attach_to_tangle_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("attachToTangle"));

  ret = flex_hash_to_json_string(json_root, "trunkTransaction", obj->trunk);
  if (ret != RC_OK) {
    goto done;
  }

  ret = flex_hash_to_json_string(json_root, "branchTransaction", obj->branch);
  if (ret != RC_OK) {
    goto done;
  }

  cJSON_AddItemToObject(json_root, "minWeightMagnitude",
                        cJSON_CreateNumber(obj->mwm));

  ret = flex_hash_array_to_json_array(obj->trytes, json_root, "trytes");
  if (ret != RC_OK) {
    goto done;
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
done:
  cJSON_Delete(json_root);
  return ret;
}

retcode_t json_attach_to_tangle_deserialize_response(
    const serializer_t* const s, const char* const obj,
    attach_to_tangle_res_t** out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;
  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);

  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  *out = json_array_to_flex_hash_array(json_obj, "trytes", *out);
  if (*out == NULL) {
    return RC_CCLIENT_FLEX_TRITS;
  }

  cJSON_Delete(json_obj);
  return ret;
}

// broadcast_transactions_request
retcode_t json_broadcast_transactions_serialize_request(
    const serializer_t* const s, broadcast_transactions_req_t* const req,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("broadcastTransactions"));

  ret = flex_hash_array_to_json_array(req->trytes, json_root, "trytes");
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

retcode_t json_store_transactions_serialize_request(
    const serializer_t* const s, store_transactions_req_t* const req,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("storeTransactions"));

  ret = flex_hash_array_to_json_array(req->trytes, json_root, "trytes");
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

retcode_t json_check_consistency_serialize_request(
    const serializer_t* const s, check_consistency_req_t* const obj,
    char_buffer_t* out) {
  retcode_t ret = RC_OK;
  const char* json_text = NULL;
  size_t len = 0;
  log_info(JSON_LOGGER_ID, "[%s:%d]\n", __func__, __LINE__);
  cJSON* json_root = cJSON_CreateObject();
  if (json_root == NULL) {
    log_critical(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
                 STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, "command",
                        cJSON_CreateString("checkConsistency"));

  ret = flex_hash_array_to_json_array(obj, json_root, "tails");
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

retcode_t json_check_consistency_deserialize_response(
    const serializer_t* const s, const char* const obj,
    check_consistency_res_t* out) {
  retcode_t ret = RC_OK;
  cJSON* json_obj = cJSON_Parse(obj);
  cJSON* json_item = NULL;

  log_info(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__, obj);
  if (json_obj == NULL) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s\n", __func__, __LINE__,
              STR_CCLIENT_JSON_PARSE);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_JSON_PARSE;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "error");
  if (cJSON_IsString(json_item) && (json_item->valuestring != NULL)) {
    log_error(JSON_LOGGER_ID, "[%s:%d] %s %s\n", __func__, __LINE__,
              STR_CCLIENT_RES_ERROR, json_item->valuestring);
    cJSON_Delete(json_obj);
    return RC_CCLIENT_RES_ERROR;
  }

  json_item = cJSON_GetObjectItemCaseSensitive(json_obj, "state");
  out->state = cJSON_IsTrue(json_item);
  ret = json_get_string(json_obj, "info", out->info);

  cJSON_Delete(json_obj);
  return ret;
}
