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
#ifndef CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H
#define CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common/errors.h"

#include "cclient/request/get_inclusion_states.h"
#include "cclient/response/get_inclusion_states.h"
#include "cclient/serialization/json/helpers.h"
#include "cclient/serialization/serializer.h"

/**
 * @brief Converts a get inclusion states request to a JSON string.
 *
 * @param[in] req A get inclusion states request object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_inclusion_states_serialize_request(get_inclusion_states_req_t const* const req, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get inclusion states request.
 *
 * @param[in] obj A JSON string.
 * @param[out] req A get inclusion states request object.
 * @return #retcode_t
 */
retcode_t json_get_inclusion_states_deserialize_request(char const* const obj, get_inclusion_states_req_t* const req);

/**
 * @brief Converts a get inclusion states response to a JSON string.
 *
 * @param[in] res A get inclusion states response object.
 * @param[out] out A JSON string.
 * @return #retcode_t
 */
retcode_t json_get_inclusion_states_serialize_response(get_inclusion_states_res_t const* const res, char_buffer_t* out);

/**
 * @brief Converts a JSON string to a get inclusion states response.
 *
 * @param[in] obj A JSON string.
 * @param[out] res A get inclusion states response object.
 * @return #retcode_t
 */
retcode_t json_get_inclusion_states_deserialize_response(char const* const obj, get_inclusion_states_res_t* const res);

#ifdef __cplusplus
}
#endif

#endif  // CCLIENT_SERIALIZATION_JSON_GET_INCLUSION_STATES_H

/** @} */