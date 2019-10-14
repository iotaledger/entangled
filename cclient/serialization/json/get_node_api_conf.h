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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_NODE_CONF_H
#define CCLIENT_SERIALIZATION_JSON_GET_NODE_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/response/get_node_api_conf.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get node configuration request into a JSON string.
 *
 * @param[out] out A JSON string of the getNodeAPIConfiguration command.
 * @return #retcode_t
 */
retcode_t json_get_node_api_conf_serialize_request(char_buffer_t* out);

/**
 * @brief Converts a get node configuration response into a JSON string.
 *
 * @param[in] obj A get node configuration response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_node_api_conf_serialize_response(get_node_api_conf_res_t const* const obj, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get node configuration response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get node configuration response object.
 * @return #retcode_t
 */
retcode_t json_get_node_api_conf_deserialize_response(char const* const obj, get_node_api_conf_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_NODE_CONF_H

/** @} */