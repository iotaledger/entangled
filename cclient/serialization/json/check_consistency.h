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
 * @brief
 *
 */
#ifndef CCLIENT_SERIALIZATION_JSON_CHECK_CONSISTENCY_H
#define CCLIENT_SERIALIZATION_JSON_CHECK_CONSISTENCY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/check_consistency.h"
#include "cclient/response/check_consistency.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a check consistency request to a JSON string.
 *
 * @param[in] obj A check consistency request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_check_consistency_serialize_request(check_consistency_req_t const *const obj, char_buffer_t *out);

/**
 * @brief Converts a check consistency response to a JSON string.
 *
 * @param[in] obj A check consistency response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_check_consistency_serialize_response(check_consistency_res_t *const obj, char_buffer_t *out);

/**
 * @brief Converts a JSON string to a check consistency request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A check consistency request object.
 * @return #retcode_t
 */
retcode_t json_check_consistency_deserialize_request(char const *const obj, check_consistency_req_t *out);

/**
 * @brief Converts a JSON string to a check consistency response.
 *
 * @param[in] obj A JSON string.
 * @param[out] out A check consistency response object.
 * @return #retcode_t
 */
retcode_t json_check_consistency_deserialize_response(char const *const obj, check_consistency_res_t *out);

#ifdef __cplusplus
}
#endif

#endif

/** @} */