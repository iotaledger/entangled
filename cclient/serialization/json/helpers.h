/*
 * Copyright (c) 2019 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

/**
 * @ingroup serialization_json
 *
 * @{
 *
 * @file
 * @brief Helper methods for JSON serializations.
 *
 */
#ifndef CCLIENT_SERIALIZATION_JSON_HELPERS_H_
#define CCLIENT_SERIALIZATION_JSON_HELPERS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "cJSON.h"
#include "common/errors.h"
#include "utarray.h"
#include "utils/char_buffer.h"
#include "utils/containers/hash/hash243_queue.h"
#include "utils/containers/hash/hash243_stack.h"
#include "utils/containers/hash/hash8019_queue.h"
#include "utils/containers/hash/hash8019_stack.h"
#include "utils/containers/hash/hash81_queue.h"
#include "utils/containers/hash/hash_array.h"
#include "utils/logger_helper.h"

/**
 * @brief Checks if the JSON object is NULL and if containing an error or exception.
 *
 */
#define JSON_CHECK_ERROR(OBJ, ITEM, LOGGER)                                                               \
  do {                                                                                                    \
    if (OBJ == NULL) {                                                                                    \
      log_error(LOGGER, "[%s:%d] %s\n", __func__, __LINE__, STR_CCLIENT_JSON_PARSE);                      \
      cJSON_Delete(OBJ);                                                                                  \
      return RC_CCLIENT_JSON_PARSE;                                                                       \
    }                                                                                                     \
    ITEM = cJSON_GetObjectItemCaseSensitive(OBJ, "error");                                                \
    if (cJSON_IsString(ITEM) && (ITEM->valuestring != NULL)) {                                            \
      log_error(LOGGER, "[%s:%d] %s %s\n", __func__, __LINE__, STR_CCLIENT_RES_ERROR, ITEM->valuestring); \
      cJSON_Delete(OBJ);                                                                                  \
      return RC_CCLIENT_RES_ERROR;                                                                        \
    }                                                                                                     \
    ITEM = cJSON_GetObjectItemCaseSensitive(OBJ, "exception");                                            \
    if (cJSON_IsString(ITEM) && (ITEM->valuestring != NULL)) {                                            \
      log_error(LOGGER, "[%s:%d] %s %s\n", __func__, __LINE__, STR_CCLIENT_RES_ERROR, ITEM->valuestring); \
      cJSON_Delete(OBJ);                                                                                  \
      return RC_CCLIENT_RES_ERROR;                                                                        \
    }                                                                                                     \
  } while (0);

/**
 * @brief Converts A JSON array to unsigned int array.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] ut An utarray object.
 * @return #retcode_t
 */
retcode_t json_array_to_uint64(cJSON const *const obj, char const *const obj_name, UT_array *ut);

/**
 * @brief Converts An utarray to A JSON array.
 *
 * @param[in] ut An utarray object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t utarray_to_json_array(UT_array const *const ut, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts unsigned int array to A JSON array.
 *
 * @param[in] ut An utarray object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t uint64_utarray_to_json_array(UT_array const *const ut, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a JSON array of booleans to an utarray.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] ut An utarray object.
 * @return #retcode_t
 */
retcode_t json_boolean_array_to_utarray(cJSON const *const obj, char const *const obj_name, UT_array *const ut);

/**
 * @brief Converts an utarray to a JSON array of booleans.
 *
 * @param[in] ut An utarray object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t utarray_to_json_boolean_array(UT_array const *const ut, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a JSON array of strings to an utarray.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] ut An utarray object.
 * @return #retcode_t
 */
retcode_t json_string_array_to_utarray(cJSON const *const obj, char const *const obj_name, UT_array *const ut);

/**
 * @brief Gets an integer from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] num A value of integer.
 * @return #retcode_t
 */
retcode_t json_get_int(cJSON const *const json_obj, char const *const obj_name, int *const num);

/**
 * @brief Gets an uint8_t from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] num A value of uint8_t.
 * @return #retcode_t
 */
retcode_t json_get_uint8(cJSON const *const json_obj, char const *const obj_name, uint8_t *const num);

/**
 * @brief Gets an uint16_t from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] num A value of uint16_t.
 * @return #retcode_t
 */
retcode_t json_get_uint16(cJSON const *const json_obj, char const *const obj_name, uint16_t *const num);

/**
 * @brief Gets an uint32_t from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] num A value of uint32_t.
 * @return #retcode_t
 */
retcode_t json_get_uint32(cJSON const *const json_obj, char const *const obj_name, uint32_t *const num);

/**
 * @brief Gets an uint64_t from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] num A value of uint64_t.
 * @return #retcode_t
 */
retcode_t json_get_uint64(cJSON const *const json_obj, char const *const obj_name, uint64_t *const num);

/**
 * @brief Gets a string from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] text A string.
 * @return #retcode_t
 */
retcode_t json_get_string(cJSON const *const json_obj, char const *const obj_name, char_buffer_t *const text);

/**
 * @brief Gets a boolean value from a JSON object.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] boolean bool.
 * @return #retcode_t
 */
retcode_t json_get_boolean(cJSON const *const json_obj, char const *const obj_name, bool *const boolean);

/**
 * @brief Converts a hash243 queue to a JSON array.
 *
 * @param[in] queue A hash243 queue object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash243_queue_to_json_array(hash243_queue_t queue, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a hash243 stack to a JSON array.
 *
 * @param[in] stack A hash243 stack object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash243_stack_to_json_array(hash243_stack_t stack, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a JSON array to a hash243 queue.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] queue A hash243 queue.
 * @return #retcode_t
 */
retcode_t json_array_to_hash243_queue(cJSON const *const obj, char const *const obj_name, hash243_queue_t *queue);

/**
 * @brief Converts a JSON array to a hash243 stack.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] stack A hash243 stack.
 * @return #retcode_t
 */
retcode_t json_array_to_hash243_stack(cJSON const *const obj, char const *const obj_name, hash243_stack_t *stack);

/**
 * @brief Converts a hash81 queue to a JSON array.
 *
 * @param[in] queue A hash81 queue object.
 * @param[in] json_root The root of a JSON object.
 * @param[out] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash81_queue_to_json_array(hash81_queue_t queue, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a JSON array to a hash81 queue.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] queue A hash81 queue.
 * @return #retcode_t
 */
retcode_t json_array_to_hash81_queue(cJSON const *const obj, char const *const obj_name, hash81_queue_t *queue);

/**
 * @brief Converts a JSON array to a hash8019 queue.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] queue A hash8019 queue.
 * @return #retcode_t
 */
retcode_t json_array_to_hash8019_queue(cJSON const *const obj, char const *const obj_name, hash8019_queue_t *queue);

/**
 * @brief Converts flex trits to a JSON element.
 *
 * @param[in, out] json_obj A JSON object.
 * @param[in] key A key of a JSON element.
 * @param[in] hash A flex trits hash.
 * @param[in] num_trits The size of flex trits hash.
 * @return #retcode_t
 */
retcode_t flex_trits_to_json_string(cJSON *const json_obj, char const *const key, flex_trit_t const *const hash,
                                    size_t num_trits);

/**
 * @brief Converts a JSON element to flex trits hash.
 *
 * @param[in] json_obj A JSON object.
 * @param[in] key A key of a JSON element.
 * @param[out] hash A flex trits hash.
 * @return #retcode_t
 */
retcode_t json_string_hash_to_flex_trits(cJSON const *const json_obj, char const *const key, flex_trit_t *hash);

/**
 * @brief Converts a hash8019 queue to a JSON array.
 *
 * @param[in] queue A hash8019 queue object.
 * @param[in, out] json_root The root of a JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash8019_queue_to_json_array(hash8019_queue_t queue, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a hash8019 stack to a JSON array.
 *
 * @param[in] stack A hash8019 stack object.
 * @param[in, out] json_root The root of a JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash8019_stack_to_json_array(hash8019_stack_t stack, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a hash8019 array to a JSON array.
 *
 * @param[in] queue A hash8019 array object.
 * @param[in, out] json_root The root of a JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @return #retcode_t
 */
retcode_t hash8019_array_to_json_array(hash8019_array_p array, cJSON *const json_root, char const *const obj_name);

/**
 * @brief Converts a JSON array to a hash8019 array.
 *
 * @param[in] obj A JSON object.
 * @param[in] obj_name A key of a JSON element.
 * @param[out] array A hash8019 array object.
 * @return #retcode_t
 */
retcode_t json_array_to_hash8019_array(cJSON const *const obj, char const *const obj_name, hash8019_array_p array);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_HELPERS_H_

/** @} */