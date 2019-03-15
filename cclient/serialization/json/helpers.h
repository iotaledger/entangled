/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef CCLIENT_SERIALIZATION_JSON_HELPERS_H_
#define CCLIENT_SERIALIZATION_JSON_HELPERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "cJSON.h"
#include "utarray.h"

#include "cclient/types/types.h"
#include "common/errors.h"

#define JSON_CHECK_ERROR(OBJ, ITEM, LOGGER)                    \
  do {                                                         \
    if (OBJ == NULL) {                                         \
      log_error(LOGGER, "[%s:%d] %s\n", __func__, __LINE__,    \
                STR_CCLIENT_JSON_PARSE);                       \
      cJSON_Delete(OBJ);                                       \
      return RC_CCLIENT_JSON_PARSE;                            \
    }                                                          \
    ITEM = cJSON_GetObjectItemCaseSensitive(OBJ, "error");     \
    if (cJSON_IsString(ITEM) && (ITEM->valuestring != NULL)) { \
      log_error(LOGGER, "[%s:%d] %s %s\n", __func__, __LINE__, \
                STR_CCLIENT_RES_ERROR, ITEM->valuestring);     \
      cJSON_Delete(OBJ);                                       \
      return RC_CCLIENT_RES_ERROR;                             \
    }                                                          \
    ITEM = cJSON_GetObjectItemCaseSensitive(OBJ, "exception"); \
    if (cJSON_IsString(ITEM) && (ITEM->valuestring != NULL)) { \
      log_error(LOGGER, "[%s:%d] %s %s\n", __func__, __LINE__, \
                STR_CCLIENT_RES_ERROR, ITEM->valuestring);     \
      cJSON_Delete(OBJ);                                       \
      return RC_CCLIENT_RES_ERROR;                             \
    }                                                          \
  } while (0);

retcode_t json_array_to_uint64(cJSON const *const obj,
                               char const *const obj_name, UT_array *ut);

retcode_t utarray_to_json_array(UT_array const *const ut,
                                cJSON *const json_root,
                                char const *const obj_name);

retcode_t uint64_utarray_to_json_array(UT_array const *const ut,
                                       cJSON *const json_root,
                                       char const *const obj_name);

retcode_t json_boolean_array_to_utarray(cJSON const *const obj,
                                        char const *const obj_name,
                                        UT_array *const ut);

retcode_t json_string_array_to_utarray(cJSON const *const obj,
                                       char const *const obj_name,
                                       UT_array *const ut);

retcode_t json_get_int(cJSON const *const json_obj, char const *const obj_name,
                       int *const num);

retcode_t json_get_uint8(cJSON const *const json_obj,
                         char const *const obj_name, uint8_t *const num);

retcode_t json_get_uint16(cJSON const *const json_obj,
                          char const *const obj_name, uint16_t *const num);

retcode_t json_get_uint32(cJSON const *const json_obj,
                          char const *const obj_name, uint32_t *const num);

retcode_t json_get_uint64(cJSON const *const json_obj,
                          char const *const obj_name, uint64_t *const num);

retcode_t json_get_string(cJSON const *const json_obj,
                          char const *const obj_name,
                          char_buffer_t *const text);

retcode_t hash243_queue_to_json_array(hash243_queue_t queue,
                                      cJSON *const json_root,
                                      char const *const obj_name);

retcode_t json_array_to_hash243_queue(cJSON const *const obj,
                                      char const *const obj_name,
                                      hash243_queue_t *queue);

retcode_t json_array_to_hash243_stack(cJSON const *const obj,
                                      char const *const obj_name,
                                      hash243_stack_t *queue);

retcode_t hash81_queue_to_json_array(hash81_queue_t queue,
                                     cJSON *const json_root,
                                     char const *const obj_name);

retcode_t json_array_to_hash8019_queue(cJSON const *const obj,
                                       char const *const obj_name,
                                       hash8019_queue_t *queue);

retcode_t flex_trits_to_json_string(cJSON *const json_obj,
                                    char const *const key,
                                    flex_trit_t const *const hash,
                                    size_t num_trits);

retcode_t json_string_hash_to_flex_trits(cJSON const *const json_obj,
                                         char const *const key,
                                         flex_trit_t *hash);

retcode_t hash8019_queue_to_json_array(hash8019_queue_t queue,
                                       cJSON *const json_root,
                                       char const *const obj_name);

retcode_t hash8019_stack_to_json_array(hash8019_stack_t stack,
                                       cJSON *const json_root,
                                       char const *const obj_name);

retcode_t hash8019_array_to_json_array(hash8019_array_p array,
                                       cJSON *const json_root,
                                       char const *const obj_name);

retcode_t json_array_to_hash8019_array(cJSON const *const obj,
                                       char const *const obj_name,
                                       hash8019_array_p array);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_HELPERS_H_
