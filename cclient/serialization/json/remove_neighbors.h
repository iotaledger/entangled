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
#ifndef CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H
#define CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/remove_neighbors.h"
#include "cclient/response/remove_neighbors.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a remove neighbors request to a JSON string.
 *
 * @param[in] req A remove neighbors request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_remove_neighbors_serialize_request(remove_neighbors_req_t const* const req, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a remove neighbors request.
 *
 * @param[in] obj A JSON string.
 * @param[out] req A remove neighbors request object.
 * @return #retcode_t
 */
retcode_t json_remove_neighbors_deserialize_request(char const* const obj, remove_neighbors_req_t* req);

/**
 * @brief Converts a remove neighbors response to a JSON string.
 *
 * @param[in] res A remove neighbors response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_remove_neighbors_serialize_response(remove_neighbors_res_t const* const res, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a remove neighbors response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A remove neighbors response object.
 * @return #retcode_t
 */
retcode_t json_remove_neighbors_deserialize_response(char const* const obj, remove_neighbors_res_t* res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_REMOVE_NEIGHBORS_H

/** @} */