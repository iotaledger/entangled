/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "cclient/serialization/json/helpers.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "utarray.h"

#include "cclient/serialization/json/logger.h"
#include "common/model/transaction.h"
#include "utils/logger_helper.h"

retcode_t json_array_to_uint64(cJSON const* const obj, char const* const obj_name, UT_array* ut) {
  char* endptr;
  uint64_t value = 0;
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        value = strtoll(current_obj->valuestring, &endptr, 10);
        utarray_push_back(ut, &value);
      }
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}

retcode_t utarray_to_json_array(UT_array const* const ut, cJSON* const json_root, char const* const obj_name) {
  cJSON* array_obj = cJSON_CreateArray();
  char** p = NULL;

  if (!ut) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_NULL_PARAM);
    return RC_NULL_PARAM;
  }

  if (array_obj == NULL) {
    log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, obj_name, array_obj);

  while ((p = (char**)utarray_next(ut, p))) {
    cJSON_AddItemToArray(array_obj, cJSON_CreateString(*p));
  }
  return RC_OK;
}

retcode_t uint64_utarray_to_json_array(UT_array const* const ut, cJSON* const json_root, char const* const obj_name) {
  if (!ut) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_NULL_PARAM);
    return RC_NULL_PARAM;
  }
  if (utarray_len(ut) > 0) {
    cJSON* array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
      return RC_CCLIENT_JSON_CREATE;
    }

    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    uint64_t* p = NULL;
    char buffer[20];
    while ((p = (uint64_t*)utarray_next(ut, p))) {
      sprintf(buffer, "%" PRIu64, *p);
      cJSON_AddItemToArray(array_obj, cJSON_CreateString(buffer));
    }
  }
  return RC_OK;
}

retcode_t json_boolean_array_to_utarray(cJSON const* const obj, char const* const obj_name, UT_array* const ut) {
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    /* The type of json object is expected to be boolean type */
    cJSON_ArrayForEach(current_obj, json_item) {
      int bl = (int)cJSON_IsTrue(current_obj);
      utarray_push_back(ut, &bl);
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}

retcode_t utarray_to_json_boolean_array(UT_array const* const ut, cJSON* const json_root, char const* const obj_name) {
  if (!ut) {
    log_error(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_NULL_PARAM);
    return RC_NULL_PARAM;
  }
  if (utarray_len(ut) > 0) {
    cJSON* array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      log_critical(json_logger_id, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_CREATE);
      return RC_CCLIENT_JSON_CREATE;
    }

    cJSON_AddItemToObject(json_root, obj_name, array_obj);
    int* p = NULL;
    while ((p = (int*)utarray_next(ut, p))) {
      cJSON_AddItemToArray(array_obj, cJSON_CreateBool(*p));
    }
  }
  return RC_OK;
}

retcode_t json_string_array_to_utarray(cJSON const* const obj, char const* const obj_name, UT_array* const ut) {
  char* str = NULL;

  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      str = cJSON_GetStringValue(current_obj);
      if (!str) {
        log_error(json_logger_id, "[%s:%d] encountered non-string array member", __func__, __LINE__);
        return RC_CCLIENT_JSON_PARSE;
      }

      utarray_push_back(ut, &str);
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_int(cJSON const* const json_obj, char const* const obj_name, int* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value)) {
    *num = json_value->valueint;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not number\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_uint8(cJSON const* const json_obj, char const* const obj_name, uint8_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value) && json_value->valueint >= 0) {
    *num = (uint8_t)json_value->valueint;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not number\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_uint16(cJSON const* const json_obj, char const* const obj_name, uint16_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value) && json_value->valueint >= 0) {
    *num = (uint16_t)json_value->valueint;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not number\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_uint32(cJSON const* const json_obj, char const* const obj_name, uint32_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value) && json_value->valuedouble >= 0) {
    *num = (uint32_t)json_value->valuedouble;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not number\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_uint64(cJSON const* const json_obj, char const* const obj_name, uint64_t* const num) {
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsNumber(json_value) && json_value->valuedouble >= 0) {
    *num = (uint64_t)json_value->valuedouble;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not number\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return RC_OK;
}

retcode_t json_get_string(cJSON const* const json_obj, char const* const obj_name, char_buffer_t* const text) {
  retcode_t ret = RC_ERROR;
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, obj_name);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, obj_name);
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    ret = char_buffer_set(text, json_value->valuestring);
    if (ret != RC_OK) {
      log_error(json_logger_id, "[%s:%d] memory allocation failed.\n", __func__, __LINE__);
      return ret;
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not string\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }

  return ret;
}

retcode_t hash243_queue_to_json_array(hash243_queue_t queue, cJSON* const json_root, char const* const obj_name) {
  size_t array_count;
  cJSON* array_obj = NULL;
  hash243_queue_entry_t* q_iter = NULL;

  array_count = hash243_queue_count(queue);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    CDL_FOREACH(queue, q_iter) {
      tryte_t trytes_out[NUM_TRYTES_HASH + 1];
      size_t trits_count =
          flex_trits_to_trytes(trytes_out, NUM_TRYTES_HASH, q_iter->hash, NUM_TRITS_HASH, NUM_TRITS_HASH);
      trytes_out[NUM_TRYTES_HASH] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t hash243_stack_to_json_array(hash243_stack_t stack, cJSON* const json_root, char const* const obj_name) {
  cJSON* array_obj = cJSON_CreateArray();
  hash243_stack_entry_t* iter = NULL;
  tryte_t trytes_out[NUM_TRYTES_HASH + 1];

  if (array_obj == NULL) {
    return RC_CCLIENT_JSON_CREATE;
  }

  cJSON_AddItemToObject(json_root, obj_name, array_obj);

  trytes_out[NUM_TRYTES_HASH] = '\0';
  HASH_STACK_FOREACH(stack, iter) {
    size_t trits_count = flex_trits_to_trytes(trytes_out, NUM_TRYTES_HASH, iter->hash, NUM_TRITS_HASH, NUM_TRITS_HASH);
    if (trits_count != 0) {
      cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
    } else {
      return RC_CCLIENT_FLEX_TRITS;
    }
  }
  return RC_OK;
}

retcode_t json_array_to_hash243_queue(cJSON const* const obj, char const* const obj_name, hash243_queue_t* queue) {
  retcode_t ret_code = RC_ERROR;
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (!json_item) {
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)current_obj->valuestring, NUM_TRYTES_HASH,
                               NUM_TRYTES_HASH);
        ret_code = hash243_queue_push(queue, hash);
        if (ret_code) {
          return ret_code;
        }
      }
    }
    ret_code = RC_OK;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret_code;
}

retcode_t json_array_to_hash243_stack(cJSON const* const obj, char const* const obj_name, hash243_stack_t* stack) {
  retcode_t ret_code = RC_ERROR;
  flex_trit_t hash[FLEX_TRIT_SIZE_243] = {};
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)current_obj->valuestring, NUM_TRYTES_HASH,
                               NUM_TRYTES_HASH);
        ret_code = hash243_stack_push(stack, hash);
        if (ret_code) {
          return ret_code;
        }
      }
    }
    ret_code = RC_OK;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret_code;
}

retcode_t hash81_queue_to_json_array(hash81_queue_t queue, cJSON* const json_root, char const* const obj_name) {
  size_t array_count;
  cJSON* array_obj = NULL;
  hash81_queue_entry_t* q_iter = NULL;

  array_count = hash81_queue_count(queue);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    CDL_FOREACH(queue, q_iter) {
      tryte_t trytes_out[NUM_TRYTES_TAG + 1];
      size_t trits_count = flex_trits_to_trytes(trytes_out, NUM_TRYTES_TAG, q_iter->hash, NUM_TRITS_TAG, NUM_TRITS_TAG);
      trytes_out[NUM_TRYTES_TAG] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t json_array_to_hash81_queue(cJSON const* const obj, char const* const obj_name, hash81_queue_t* queue) {
  retcode_t ret_code = RC_ERROR;
  flex_trit_t hash[FLEX_TRIT_SIZE_81] = {};
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (!json_item) {
    return RC_CCLIENT_JSON_KEY;
  }

  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        flex_trits_from_trytes(hash, NUM_TRITS_TAG, (tryte_t const*)current_obj->valuestring, NUM_TRYTES_TAG,
                               NUM_TRYTES_TAG);
        ret_code = hash81_queue_push(queue, hash);
        if (ret_code) {
          return ret_code;
        }
      }
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret_code;
}

retcode_t json_array_to_hash8019_queue(cJSON const* const obj, char const* const obj_name, hash8019_queue_t* queue) {
  retcode_t ret_code = RC_ERROR;
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t const*)current_obj->valuestring,
                               NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
        ret_code = hash8019_queue_push(queue, hash);
        if (ret_code) {
          return ret_code;
        }
      }
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return ret_code;
}

retcode_t flex_trits_to_json_string(cJSON* const json_obj, char const* const key, flex_trit_t const* const hash,
                                    size_t num_trits) {
  // flex to trytes;
  size_t len_trytes = num_trits / 3;
  tryte_t trytes_out[len_trytes + 1];
  size_t trits_count = flex_trits_to_trytes(trytes_out, len_trytes, hash, num_trits, num_trits);
  trytes_out[len_trytes] = '\0';

  if (trits_count != 0) {
    cJSON_AddItemToObject(json_obj, key, cJSON_CreateString((char const*)trytes_out));
  } else {
    return RC_CCLIENT_FLEX_TRITS;
  }
  return RC_OK;
}

retcode_t json_string_hash_to_flex_trits(cJSON const* const json_obj, char const* const key, flex_trit_t* hash) {
  size_t trit_len = 0;
  cJSON* json_value = cJSON_GetObjectItemCaseSensitive(json_obj, key);
  if (json_value == NULL) {
    log_error(json_logger_id, "[%s:%d] %s %s.\n", __func__, __LINE__, STR_CCLIENT_JSON_KEY, key);
    return RC_CCLIENT_JSON_KEY;
  }
  if (cJSON_IsString(json_value) && (json_value->valuestring != NULL)) {
    trit_len = flex_trits_from_trytes(hash, NUM_TRITS_HASH, (tryte_t const*)json_value->valuestring, NUM_TRYTES_HASH,
                                      NUM_TRYTES_HASH);
    if (!trit_len) {
      return RC_CCLIENT_FLEX_TRITS;
    }
    return RC_OK;
  } else {
    log_error(json_logger_id, "[%s:%d] %s not string\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
}

retcode_t hash8019_queue_to_json_array(hash8019_queue_t queue, cJSON* const json_root, char const* const obj_name) {
  size_t array_count = 0;
  cJSON* array_obj = NULL;
  hash8019_queue_entry_t* q_iter = NULL;
  tryte_t trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];
  size_t trits_count = 0;

  array_count = hash8019_queue_count(queue);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    CDL_FOREACH(queue, q_iter) {
      trits_count = flex_trits_to_trytes(trytes_out, NUM_TRYTES_SERIALIZED_TRANSACTION, q_iter->hash,
                                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
      trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t hash8019_stack_to_json_array(hash8019_stack_t stack, cJSON* const json_root, char const* const obj_name) {
  size_t array_count = 0;
  cJSON* array_obj = NULL;
  hash8019_stack_entry_t* s_iter = NULL;
  tryte_t trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION + 1];
  size_t trits_count = 0;

  array_count = hash8019_stack_count(stack);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    LL_FOREACH(stack, s_iter) {
      trits_count = flex_trits_to_trytes(trytes_out, NUM_TRYTES_SERIALIZED_TRANSACTION, s_iter->hash,
                                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
      trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t hash8019_array_to_json_array(hash8019_array_p array, cJSON* const json_root, char const* const obj_name) {
  size_t array_count = 0;
  cJSON* array_obj = NULL;
  tryte_t trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION + 1] = {};
  size_t trits_count = 0;
  flex_trit_t* elt = NULL;

  array_count = hash_array_len(array);
  if (array_count > 0) {
    array_obj = cJSON_CreateArray();
    if (array_obj == NULL) {
      return RC_CCLIENT_JSON_CREATE;
    }
    cJSON_AddItemToObject(json_root, obj_name, array_obj);

    HASH_ARRAY_FOREACH(array, elt) {
      trits_count = flex_trits_to_trytes(trytes_out, NUM_TRYTES_SERIALIZED_TRANSACTION, elt,
                                         NUM_TRITS_SERIALIZED_TRANSACTION, NUM_TRITS_SERIALIZED_TRANSACTION);
      trytes_out[NUM_TRYTES_SERIALIZED_TRANSACTION] = '\0';
      if (trits_count != 0) {
        cJSON_AddItemToArray(array_obj, cJSON_CreateString((char const*)trytes_out));
      } else {
        return RC_CCLIENT_FLEX_TRITS;
      }
    }
  }
  return RC_OK;
}

retcode_t json_array_to_hash8019_array(cJSON const* const obj, char const* const obj_name, hash8019_array_p array) {
  flex_trit_t hash[FLEX_TRIT_SIZE_8019] = {};
  cJSON* json_item = cJSON_GetObjectItemCaseSensitive(obj, obj_name);
  if (cJSON_IsArray(json_item)) {
    cJSON* current_obj = NULL;
    cJSON_ArrayForEach(current_obj, json_item) {
      if (current_obj->valuestring != NULL) {
        flex_trits_from_trytes(hash, NUM_TRITS_SERIALIZED_TRANSACTION, (tryte_t const*)current_obj->valuestring,
                               NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
        hash_array_push(array, hash);
      }
    }
  } else {
    log_error(json_logger_id, "[%s:%d] %s not array\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);
    return RC_CCLIENT_JSON_PARSE;
  }
  return RC_OK;
}
