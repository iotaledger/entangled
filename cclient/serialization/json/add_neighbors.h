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
 * @brief add_neighbors request object to JSON string
 *
 * @param obj add_neighbors request object
 * @param out JSON string
 * @return error code
 */
retcode_t json_add_neighbors_serialize_request(const add_neighbors_req_t* const obj, char_buffer_t* out);

/**
 * @brief add_neighbors response object to JSON string
 *
 * @param obj add_neighbors response object
 * @param out JSON string
 * @return error code
 */
retcode_t json_add_neighbors_serialize_response(const add_neighbors_res_t* const obj, char_buffer_t* out);

/**
 * @brief JSON string to add_neighbors request object
 *
 * @param obj JSON string
 * @param out add_neighbors request object
 * @return error code
 */
retcode_t json_add_neighbors_deserialize_request(const char* const obj, add_neighbors_req_t* out);

/**
 * @brief JSON string to add_neighbors response object
 *
 * @param obj JSON string
 * @param out add_neighbors response object
 * @return error code
 */
retcode_t json_add_neighbors_deserialize_response(const char* const obj, add_neighbors_res_t* out);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_ADD_NEIGHBORS

/** @} */
