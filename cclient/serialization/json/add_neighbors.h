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
#ifndef CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS_H
#define CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/add_neighbors.h"
#include "cclient/response/add_neighbors.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts an add neighbors request object to a JSON string.
 *
 * @param[in] obj An add neighbors request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_add_neighbors_serialize_request(add_neighbors_req_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts an add neighbors response object to a JSON string
 *
 * @param[in] obj An add neighbors response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_add_neighbors_serialize_response(add_neighbors_res_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to an add neighbors request.
 *
 * @param[in] obj A JSON string.
 * @param[out] out An add neighbors request object.
 * @return #retcode_t
 */
retcode_t json_add_neighbors_deserialize_request(char const* const obj, add_neighbors_req_t* out);

/**
 * @brief Converts a JSON string to an add neighbors response.
 *
 * @param[in] obj A JSON string.
 * @param[out] out An add neighbors response object.
 * @return #retcode_t
 */
retcode_t json_add_neighbors_deserialize_response(char const* const obj, add_neighbors_res_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS

/** @} */
