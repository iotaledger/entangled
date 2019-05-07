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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_NODE_INFO_H
#define CCLIENT_SERIALIZATION_JSON_GET_NODE_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/get_node_info.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get node info request to a JSON string.
 *
 * @param[in] obj A get node info request object.
 * @return #retcode_t
 */
retcode_t json_get_node_info_serialize_request(char_buffer_t* out);

/**
 * @brief Converts a get node info response to a JSON string.
 *
 * @param[in] obj A get node info response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_node_info_serialize_response(get_node_info_res_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get node info response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get node info response object.
 * @return #retcode_t
 */
retcode_t json_get_node_info_deserialize_response(char const* const obj, get_node_info_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_NODE_INFO_H

/** @} */