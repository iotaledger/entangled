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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_TRYTES_H
#define CCLIENT_SERIALIZATION_JSON_GET_TRYTES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/get_trytes.h"
#include "cclient/response/get_trytes.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get trytes request to a JSON string.
 *
 * @param[in] req A get trytes request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_trytes_serialize_request(get_trytes_req_t const* const req, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get trytes request.
 *
 * @param[in] obj A JSON string.
 * @param[out] req A get trytes request object.
 * @return #retcode_t
 */
retcode_t json_get_trytes_deserialize_request(char const* const obj, get_trytes_req_t* const req);

/**
 * @brief Converts a get trytes response to a JSON string.
 *
 * @param[in] res A get trytes response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_trytes_serialize_response(get_trytes_res_t const* const res, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get trytes response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get trytes response object.
 * @return #retcode_t
 */
retcode_t json_get_trytes_deserialize_response(char const* const obj, get_trytes_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_TRYTES_H

/** @} */