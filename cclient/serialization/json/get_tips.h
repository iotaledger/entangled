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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_TIPS_H
#define CCLIENT_SERIALIZATION_JSON_GET_TIPS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/get_tips.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get tips request to a JSON string.
 *
 * @param[in] obj A get tips request object.
 * @return #retcode_t
 */
retcode_t json_get_tips_serialize_request(char_buffer_t* out);

/**
 * @brief Converts a get tips response to a JSON string.
 *
 * @param[in] res A get tips response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_tips_serialize_response(get_tips_res_t const* const res, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get tips response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get tips response object.
 * @return #retcode_t
 */
retcode_t json_get_tips_deserialize_response(char const* const obj, get_tips_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_TIPS_H

/** @} */